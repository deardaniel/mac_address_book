require 'mkmf'

$CFLAGS << ' -fnested-functions'
$LDFLAGS << ' -framework Carbon -framework AddressBook'

create_makefile("mac_address_book_ext")
