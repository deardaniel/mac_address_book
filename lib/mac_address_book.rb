require 'mac_address_book_ext'

module MacAddressBook
  class Person
    def inspect
      "#<Person:#{to_s}>"
    end

    def to_s
      organization || [title, first, middle, last, suffix].compact.join(' ')
    end
    
    def properties
      @@properties
    end
  end


  class MultiValue
    def inspect
      "#<MultiValue:#{localized_label.inspect} => #{value.inspect}>"
    end

    def to_s
      value
    end
  end
  
  module Record
    def eql? o
      unique_id == o.unique_id
    end
    
    def hash
      unique_id.hash
    end
  end
  
  class Group
    def inspect
      "#<Group:#{name}>"
    end

    def to_s
      name
    end
  end
end