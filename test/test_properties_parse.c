#include <stdlib.h>
#include "unity.h"
#include "properties_parse.h"

#define FIXTURE(name) FIXTURES_DIR name

void setUp(void) {}
void tearDown(void) {}

/* Frees everything parsePropFile() allocated into *obj, mirroring what
 * getPropertyValue() does internally after it's done reading a value. */
static void freeObject(struct Object *obj) {
    for (size_t i = 0; i < obj->count; i++) {
        free(obj->fields[i].key);
        free(obj->fields[i].value);
    }
    free(obj->fields);
}

/* ---- Black-box tests of getPropertyValue() ---------------------------- */

static void test_getPropertyValue_ReturnsValueForExistingKey(void) {
    char *value = (char *)getPropertyValue("port", FIXTURE("basic.properties"));
    TEST_ASSERT_EQUAL_STRING("9082", value);
    free(value);
}

static void test_getPropertyValue_ReturnsCorrectValuesForMultipleKeys(void) {
    char *docroot = (char *)getPropertyValue("docroot", FIXTURE("basic.properties"));
    char *name = (char *)getPropertyValue("name", FIXTURE("basic.properties"));
    TEST_ASSERT_EQUAL_STRING("../www/html/", docroot);
    TEST_ASSERT_EQUAL_STRING("props-parse", name);
    free(docroot);
    free(name);
}

static void test_getPropertyValue_MissingKeyReturnsKeyNotFoundLiteral(void) {
    /* Pins CURRENT behavior: getValue() falls back to the literal string
     * "key not found" and getPropertyValue() strdup()s it as if it were a
     * real value, rather than returning NULL as readme.md claims. This is
     * a known quirk, not desired behavior - see readme.md discussion. */
    char *value = (char *)getPropertyValue("nope", FIXTURE("basic.properties"));
    TEST_ASSERT_EQUAL_STRING("key not found", value);
    free(value);
}

static void test_getPropertyValue_NonexistentFileReturnsNull(void) {
    const char *value = getPropertyValue("port", FIXTURE("does_not_exist.properties"));
    TEST_ASSERT_NULL(value);
}

static void test_getPropertyValue_EmptyFileReturnsKeyNotFoundLiteral(void) {
    char *value = (char *)getPropertyValue("anything", FIXTURE("empty.properties"));
    TEST_ASSERT_EQUAL_STRING("key not found", value);
    free(value);
}

static void test_getPropertyValue_FullLineCommentProducesNoUsableKey(void) {
    /* A line that's only a comment (no '=') still yields a spurious
     * Field{key="", value=""} rather than being skipped entirely. */
    char *value = (char *)getPropertyValue("", FIXTURE("comments.properties"));
    TEST_ASSERT_EQUAL_STRING("", value);
    free(value);
}

static void test_getPropertyValue_InlineCommentTruncatesValueAtHash(void) {
    /* "key=value # trailing inline comment" - the value stops at '#', but
     * the space between "value" and '#' is kept verbatim: nothing trims
     * trailing whitespace. */
    char *value = (char *)getPropertyValue("key", FIXTURE("comments.properties"));
    TEST_ASSERT_EQUAL_STRING("value ", value);
    free(value);
}

static void test_getPropertyValue_ExtraEqualsSignsKeptVerbatimInValue(void) {
    char *value = (char *)getPropertyValue("key", FIXTURE("extra_equals.properties"));
    TEST_ASSERT_EQUAL_STRING("a=b=c", value);
    free(value);
}

static void test_getPropertyValue_BlankLineDoesNotBreakSubsequentLookups(void) {
    char *first = (char *)getPropertyValue("first", FIXTURE("blank_lines.properties"));
    char *second = (char *)getPropertyValue("second", FIXTURE("blank_lines.properties"));
    TEST_ASSERT_EQUAL_STRING("one", first);
    TEST_ASSERT_EQUAL_STRING("two", second);
    free(first);
    free(second);
}

static void test_getPropertyValue_KeyWithSurroundingWhitespaceDoesNotMatchTrimmedKey(void) {
    /* "key = value" is stored as key="key " (trailing space kept) and
     * value=" value" (leading space kept) - nothing gets trimmed, so a
     * lookup for the trimmed key "key" does not match it. */
    char *value = (char *)getPropertyValue("key", FIXTURE("whitespace.properties"));
    TEST_ASSERT_EQUAL_STRING("key not found", value);
    free(value);
}

static void test_getPropertyValue_GrowthBeyondInitialCapacityExercisesRealloc(void) {
    /* 10 keys forces the internal fields array (initial capacity 8) to
     * grow once via realloc; confirm the first, middle, and last entries
     * all survive that correctly. */
    char *first = (char *)getPropertyValue("key0", FIXTURE("many_keys.properties"));
    char *middle = (char *)getPropertyValue("key5", FIXTURE("many_keys.properties"));
    char *last = (char *)getPropertyValue("key9", FIXTURE("many_keys.properties"));
    TEST_ASSERT_EQUAL_STRING("value0", first);
    TEST_ASSERT_EQUAL_STRING("value5", middle);
    TEST_ASSERT_EQUAL_STRING("value9", last);
    free(first);
    free(middle);
    free(last);
}

static void test_getPropertyValue_MalformedLineDoesNotBreakSubsequentLookups(void) {
    /* A line with neither '=' nor a leading '#' (garbage input) still
     * yields a spurious Field rather than aborting the parse - real keys
     * before and after it must still resolve correctly. */
    char *first = (char *)getPropertyValue("first", FIXTURE("malformed_line.properties"));
    char *second = (char *)getPropertyValue("second", FIXTURE("malformed_line.properties"));
    TEST_ASSERT_EQUAL_STRING("one", first);
    TEST_ASSERT_EQUAL_STRING("two", second);
    free(first);
    free(second);
}

/* ---- White-box tests of parsePropFile() / getValue() ------------------ */

static void test_parsePropFile_ReturnsMinusOneOnMissingFile(void) {
    struct Object obj;
    int result = parsePropFile(FIXTURE("does_not_exist.properties"), &obj);
    TEST_ASSERT_EQUAL_INT(-1, result);
}

static void test_parsePropFile_EmptyFileReturnsSuccessWithZeroCount(void) {
    struct Object obj;
    int result = parsePropFile(FIXTURE("empty.properties"), &obj);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_size_t(0, obj.count);
    freeObject(&obj);
}

static void test_parsePropFile_CountsSpuriousFieldsFromBlankAndCommentLines(void) {
    /* comments.properties has one full-line comment (no '=') and one real
     * key=value line - the comment line still produces a Field, so the
     * count is 2, not 1. */
    struct Object obj;
    int result = parsePropFile(FIXTURE("comments.properties"), &obj);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_size_t(2, obj.count);
    freeObject(&obj);
}

static void test_parsePropFile_CountMatchesExpectedAfterReallocGrowth(void) {
    struct Object obj;
    int result = parsePropFile(FIXTURE("many_keys.properties"), &obj);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_size_t(10, obj.count);
    freeObject(&obj);
}

static void test_parsePropFile_MalformedLineValueScanStartsAtIndexZero(void) {
    /* A line with no '=' never reassigns `index`, so the value-scan loop
     * starts at 0 and copies the WHOLE line (minus the trailing newline)
     * into the spurious field's value - pinning this kills any mutation
     * of that initial index value, which would otherwise silently produce
     * an empty value instead. */
    struct Object obj;
    int result = parsePropFile(FIXTURE("malformed_line.properties"), &obj);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_size_t(3, obj.count);
    TEST_ASSERT_EQUAL_STRING("not a valid line at all", obj.fields[1].value);
    freeObject(&obj);
}

static void test_getValue_ReturnsValueWhenPresent(void) {
    struct Field fields[] = {
        {(char *)"a", (char *)"1"},
        {(char *)"b", (char *)"2"},
    };
    struct Object obj = {fields, 2};
    TEST_ASSERT_EQUAL_STRING("1", getValue("a", &obj));
    TEST_ASSERT_EQUAL_STRING("2", getValue("b", &obj));
}

static void test_getValue_ReturnsKeyNotFoundLiteralWhenAbsent(void) {
    struct Field fields[] = {
        {(char *)"a", (char *)"1"},
    };
    struct Object obj = {fields, 1};
    TEST_ASSERT_EQUAL_STRING("key not found", getValue("missing", &obj));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_getPropertyValue_ReturnsValueForExistingKey);
    RUN_TEST(test_getPropertyValue_ReturnsCorrectValuesForMultipleKeys);
    RUN_TEST(test_getPropertyValue_MissingKeyReturnsKeyNotFoundLiteral);
    RUN_TEST(test_getPropertyValue_NonexistentFileReturnsNull);
    RUN_TEST(test_getPropertyValue_EmptyFileReturnsKeyNotFoundLiteral);
    RUN_TEST(test_getPropertyValue_FullLineCommentProducesNoUsableKey);
    RUN_TEST(test_getPropertyValue_InlineCommentTruncatesValueAtHash);
    RUN_TEST(test_getPropertyValue_ExtraEqualsSignsKeptVerbatimInValue);
    RUN_TEST(test_getPropertyValue_BlankLineDoesNotBreakSubsequentLookups);
    RUN_TEST(test_getPropertyValue_KeyWithSurroundingWhitespaceDoesNotMatchTrimmedKey);
    RUN_TEST(test_getPropertyValue_GrowthBeyondInitialCapacityExercisesRealloc);
    RUN_TEST(test_getPropertyValue_MalformedLineDoesNotBreakSubsequentLookups);

    RUN_TEST(test_parsePropFile_ReturnsMinusOneOnMissingFile);
    RUN_TEST(test_parsePropFile_EmptyFileReturnsSuccessWithZeroCount);
    RUN_TEST(test_parsePropFile_CountsSpuriousFieldsFromBlankAndCommentLines);
    RUN_TEST(test_parsePropFile_CountMatchesExpectedAfterReallocGrowth);
    RUN_TEST(test_parsePropFile_MalformedLineValueScanStartsAtIndexZero);
    RUN_TEST(test_getValue_ReturnsValueWhenPresent);
    RUN_TEST(test_getValue_ReturnsKeyNotFoundLiteralWhenAbsent);

    return UNITY_END();
}
