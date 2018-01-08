The group decided on using the GNU coding standard as it was thought to provide a clear block structure, increase readablity and be helpful when following a logical flow.
All function names are therefore found in the leftmost column to facilitate navigation. The tool Artistic Style was used to ensure than the code confirmed to the GNU coding standards,
and all source files are formatted with the flags

--style=gnu: function body parentheses and estructs are positioned in the leftmost column.

-xd: padding before an opening parentheses.

-p: puts a whitespace around mathematical operators.

-xC79: sets the maximal line length to 79 characters.

Additionally, the team used the tool indent with its default setting indents code to conform to the GNU coding standards.

To maintain readability, we have aspired to use descriptive macros in places where code otherwise would appear bulky.
An operation that is often performed is the convertion between data object and meta data, wich in literal code appears as ((object_record_t*)(object) - 1) and ((obj)(record + 1)).
The code is in itself not particaluary difficult to understand but is exhausting to read repeatedly. Thus the macros OBJECT_TO_RECORD and RECORD_TO_OBJECT are used.
Macros are also used in if statements when conditions would otherwise become difficult to grasp.

The code aspire to facilitate maintainence and changes by, for example, define custom formats for the datatypes used for the reference count and size of an allocation.
This way, changes can be made in a single line and impact the entire program.
An exception to this is when overflow of the reference count variable is checked against the literal max value of an unsigned short.

According to the team, the most important aspect to maintain readablity for all members has been well documented code.
The Doxygen standard was used for documenting functions, structs, definitions etc. wich contributed to making the code graspable also for members not involved in writing the code.
Keeping code block somewhat short and thereby easely surveyed was a top priority and the largest function turned out to be allocate() at 23 lines. This is justified given its central roll in the library.

During pull requests the team has consistently demanded desciptive variable and function names. Names consisting of a single character har not been accepted.
The team believes that the consequences of these factors combined is that the reading and understanding of code has been relatively painless, aside from commits of very large blocks of new code.
