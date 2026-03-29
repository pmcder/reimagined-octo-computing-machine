## Si Parse

### Synopsis

simple properties file parser

### supported files
- The parser will accept any filename. 
- The expected format is: key=value with no repeat keys
- If a repeat key is used, the value of the first occurrence will be returned with no warning.
- If more than one "=" is present in a line, the subsequent occurences will be treated as part of the value
- a comment is is acheived by `#` anything after a `#` on that line will be disregarded

### API

`getPropertyValue(char *key, char *filename)`

This function takes the key and the name of your properties file
as input and returns the value for that key.
If the properties file name is invalid or the key does not exist, NULL is returned.

### Building and using as a library




