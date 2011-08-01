mac_address_book
================

This is a simple wrapper around OS X's AddressBook framework's classes allowing access to the Address Book database from Ruby.

This is mainly an experiment to play with making Ruby interact with Apple frameworks and is not nearly ready for real-world use. I highly recommend checking out [MacRuby](http://www.macruby.org/) if you need this functionality.

TODO
----
* **Tests**
* Fix compatibility bugs with Lion (reading some properties of a Person causes a crash)

Examples
--------

### List all people

```ruby
p MacAddressBook::AddressBook.people
```

### Display the current user

```ruby
me = MacAddressBook::AddressBook.me
puts "I am #{me.to_s}"

# Person.formatted_address is an array of hashes.
# Each hash has a :label key and a :value key.
# :label is not unique to each address.
me.formatted_address.each do |address|
  puts "#{address[:label].upcase}\n----\n#{address[:value]}\n\n"
end
```

### Get vCard data for a user

```ruby
# Returns a string with vcard data
MacAddressBook::AddressBook.me.vcard_representation
```

### Change "me" to the first person in the address book, then back again

```ruby
old_me = MacAddressBook::AddressBook.me
# show first name
puts "Old me: #{old_me.first}"
# pick a new "me"
new_me = MacAddressBook::AddressBook.people.first
puts "New me should be: #{new_me.first}"
# set "me"
MacAddressBook::AddressBook.me = new_me
puts "New me is: #{MacAddressBook::AddressBook.me.first}"

puts "Resetting me to: #{old_me.first}"
MacAddressBook::AddressBook.me = old_me
puts "Me is now: #{MacAddressBook::AddressBook.me.first}"
```

### Adding a note to a user

```ruby
me = MacAddressBook::AddressBook.me
to = rand.to_s # Random string (which happens to be numerical)
puts "#{me.note.inspect} => #{to.inspect}"
me.note = to
puts " ==> #{me.note.inspect}"
```

### Show each group's parent groups and subgroups
```ruby
MacAddressBook::AddressBook.groups.each do |g|
  p "Members of #{g}:", g.members
  p "Subgroups of #{g}:", g.subgroups
end
```

Contributing to mac_address_book
-------------------------------- 
* Check out the latest master to make sure the feature hasn't been implemented or the bug hasn't been fixed yet
* Check out the issue tracker to make sure someone already hasn't requested it and/or contributed it
* Fork the project
* Start a feature/bugfix branch
* Commit and push until you are happy with your contribution
* Make sure to add tests for it. This is important so I don't break it in a future version unintentionally.
* Please try not to mess with the Rakefile, version, or history. If you want to have your own version, or is otherwise necessary, that is fine, but please isolate to its own commit so I can cherry-pick around it.

Copyright
---------
Copyright (c) 2011 Daniel Heffernan. See LICENSE.txt for
further details.

