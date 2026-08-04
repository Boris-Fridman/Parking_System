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

MAKEFLAGS += --no-print-directory

CURRENT_DIR := $(shell pwd)
TEST_VAR1            = "\\033[104m"
TEST_VAR2            = "\\033[39\;49m"

all: compallprograms

# compallprograms:
# 	@echo "$(CURRENT_DIR)"
compallprograms:
	@echo "$(TermBrightYello)Compiling all programs...$(TermColorsReset)" 
	@$(MAKE) comppriceman compparkman compcarman
# 	@echo "$(TermBrightCyan)Compiling Priceman program...$(TermColorsReset)"
# 	@$(MAKE) -C PriceMan
# 	@echo "$(TermBrightCyan)Compiling ParkMan program...$(TermColorsReset)" 
# 	@$(MAKE) -C ParkMan
# 	@echo "$(TermBrightCyan)Compiling CaraMan program...$(TermColorsReset)"
# 	@$(MAKE) -C CarMan
	@echo "$(TermBrightYello)Finished$(TermColorsReset)" 

comppriceman:
	@echo "$(TermBrightCyan)Compiling Priceman program...$(TermColorsReset)"
	@$(MAKE) -C PriceMan OUT_DIR="$(CURDIR)"

compparkman:
	@echo "$(TermBrightCyan)Compiling ParkMan program...$(TermColorsReset)" 
	@$(MAKE) -C ParkMan OUT_DIR="$(CURDIR)"

compcarman:
	@echo "$(TermBrightCyan)Compiling CaraMan program...$(TermColorsReset)"
	@$(MAKE) -C CarMan OUT_DIR="$(CURDIR)"




# compallprograms: comppricemanextprog compparkmanextprog compcarmanextprog

# comppricemanextprog: 
# 	@echo "Compiling Priceman program..."
# 	@$(MAKE) -C PriceMan

# compparkmanextprog: 
# 	@echo "Compiling ParkMan program..." 
# 	@$(MAKE) -C ParkMan

# compcarmanextprog:  
# 	@echo "Compiling CaraMan program..."
# 	@$(MAKE) -C CarMan
