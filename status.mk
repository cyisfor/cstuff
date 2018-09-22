$(file > make-status.log)
status=$(info $(statusPrefix)$(1)$(2)$(3)$(statusSuffix))$(file >> make-status.log, $(1) $(2) $(3))
# https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
statusColor=36
# cyan
statusPrefix=$(shell echo -e '\x1b[$(statusColor)m')
statusSuffix=$(shell echo -e '\x1b[0m')

test:
	$(call status, test this thing)
