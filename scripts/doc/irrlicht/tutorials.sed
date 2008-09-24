# Remove DOS newlines
#s/\r$//

# Page start and end are delimited by /** and **/
# we keep the end unchanged, the header is extended
s/\/\*\* Example \(0*\)\([0-9]*\) \(.*\)$/\/\*\* \\page example\1\2 Tutorial \2: \3\n \\image html \"\1\2shot.jpg\"\r\n \\image latex \"\1\2shot.jpg\"\r/

# All other comments start and end code sections
s/\([^\*]\)\*\//\1\\code/
s/^\*\//\\code/

s/\/\*\([^\*]\)/\\endcode \1/
s/\/\*$/\\endcode\r\n/

