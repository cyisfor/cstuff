$(file > make-status.log)
status=$(info $(statusPrefix)STATUS$(statusMidfix)$(1)$(2)$(3)$(statusSuffix))$(file >> make-status.log, $(1) $(2) $(3))
# https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
statusColor=1;34;48
statusMidColor=0;1;36;48
# cyan
statusPrefix=$(shell echo -e '\x1b[$(statusColor)m')
statusMidfix=$(shell echo -e '\x1b[$(statusMidColor)m')  
statusSuffix=  $(shell echo -e '\x1b[0m')

test:
	$(call status, test this thing)
