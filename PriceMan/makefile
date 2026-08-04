# ----------------------------------------------------------------------------------------------------------------------
# 
#   Terminal Colors
#  
#   The information about colors can be found in the sine:
#   https://en.wikipedia.org/wiki/ANSI_escape_code
#  
# ----------------------------------------------------------------------------------------------------------------------


#   Foreground Colors                                                                                                   
TermBlack           = "\\033[30m"
TermRed             = "\\033[31m"
TermGreen           = "\\033[32m"
TermYello           = "\\033[33m"
TermBlue            = "\\033[34m"
TermMagenta         = "\\033[35m"
TermCyan            = "\\033[36m"
TermWhite           = "\\033[37m"#  (Light Gray)

TermBrightBlack     = "\\033[90m"#  (Dark Gray)
TermBrightRed       = "\\033[91m"
TermBrightGreen     = "\\033[92m"
TermBrightYello     = "\\033[93m"
TermBrightBlue      = "\\033[94m"
TermBrightMagenta   = "\\033[95m"
TermBrightCyan      = "\\033[96m"
TermBrightWhite     = "\\033[97m"

# ----------------------------------------------------------------------------------------------------------------------
#  Background Colors                                                                                                   
TermBGBlack         = "\\033[40m"
TermBGRed           = "\\033[41m"
TermBGGreen         = "\\033[42m"
TermBGYello         = "\\033[43m"
TermBGBlue          = "\\033[44m"
TermBGMagenta       = "\\033[45m"
TermBGCyan          = "\\033[46m"
TermBGWhite         = "\\033[47m"#  (Light Gray)

TermBGBrightBlack   = "\\033[100m"# (Dark Gray)
TermBGBrightRed     = "\\033[101m"
TermBGBrightGreen   = "\\033[102m"
TermBGBrightYello   = "\\033[103m"
TermBGBrightBlue    = "\\033[104m"
TermBGBrightMagenta = "\\033[105m"
TermBGBrightCyan    = "\\033[106m"
TermBGBrightWhite   = "\\033[107m"


TermColorsReset     = "\\033[39\;49m"

OUT_DIR ?= .

CXX = gcc

COLORSENABLE = -fdiagnostics-color=always

#Allow debugging:
DEBUGEN = -g
#Define Optimization:
OPTLEVEL = -O2
#Standard:
STANDARD = -std=c99
#Enforce standard (no compiler specific features allowed):
ENFORCESTANDARD = -pedantic
#Warning flags:
WARNINGFLAGS = -Wall -Wextra

#Definitions:
 #Unlock GNU/Linux extensions, POSIX standards, and non-standard functions in the GNU C Library (glibc).
DEFINES = -D_GNU_SOURCE

#source files - use all top level .c files:
SRCS = \
*.c                                      \
../Common_Libs/CommonData.c              \
../Common_Libs/DataBase/PriceDataBase.c  \
../Common_Libs/Configuration/ConfMan.c

TARGET = $(OUT_DIR)/priceman.elf

#Includes:
INCLUDES = -I.\
-I./../Common_Libs                       \
-I./../Common_Libs/DataBase              \
-I./../Common_Libs/Configuration               

#Link math lib:
LINKMATH = -lm

USESQLITE = -lsqlite3
#Implementing database:
COMPFLAGS = $(COLORSENABLE) $(STANDARD) $(ENFORCESTANDARD) $(WARNINGFLAGS) $(DEFINES) $(SRCS) -o $(TARGET) $(INCLUDES) $(LINKMATH) $(USESQLITE)

#OBJS = $(SRCS:.c=.o)

# Default target that executes when you type 'make'
#all: pricemanprog
all: release


pricemanprog: 
	@echo "Starting the build process right now... $(TARGET) $(TEST)"
	@$(CXX) $(COLORSENABLE) $(DEBUGEN) $(STANDARD) $(ENFORCESTANDARD) $(WARNINGFLAGS) $(DEFINES) $(SRCS) -o $(TARGET) $(INCLUDES) $(LINKMATH) $(USESQLITE)


debug: compdebug
compdebug:
	@echo "$(TermYello)Running Debug Compilation$(TermColorsReset)"
	@$(CXX) -g -O0 -DDEBUG $(COMPFLAGS);
debug: showresult	


release: comprelease  
comprelease: 
	@echo "$(TermYello)Running Release Compilation$(TermColorsReset)"
	@$(CXX) -O3 -Werror -DRELEASE $(COMPFLAGS)
release: showresult

showresult:
	@if [ $$? -eq 0 ]; then \
		echo "$(TermBrightGreen)Success!$(TermColorsReset)"; \
	else \
		echo "$(TermBrightRed)Failed.$(TermColorsReset)"; \
	fi



clean:
	rm -f $(TARGET)
