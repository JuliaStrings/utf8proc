#  Copyright (c) 2009 Public Software Group e. V., Berlin, Germany
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.


#
#  File name:    ruby/utf8proc.rb
#
#  Description:
#  Part of the ruby wrapper for libutf8proc, which is written in ruby.
#


require 'utf8proc_native'


module Utf8Proc

  SpecialChars = {
    :HT => "\x09",
    :LF => "\x0A",
    :VT => "\x0B",
    :FF => "\x0C",
    :CR => "\x0D",
    :FS => "\x1C",
    :GS => "\x1D",
    :RS => "\x1E",
    :US => "\x1F",
    :LS => "\xE2\x80\xA8",
    :PS => "\xE2\x80\xA9",
  }

  module StringExtensions
    def utf8map(*option_array)
      options = 0
      option_array.each do |option|
        flag = Utf8Proc::Options[option]
        raise ArgumentError, "Unknown argument given to String#utf8map." unless
          flag
        options |= flag
      end
      return Utf8Proc::utf8map(self, options)
    end
    def utf8map!(*option_array)
      self.replace(self.utf8map(*option_array))
    end
    def utf8nfd;   utf8map( :stable, :decompose); end
    def utf8nfd!;  utf8map!(:stable, :decompose); end
    def utf8nfc;   utf8map( :stable, :compose); end
    def utf8nfc!;  utf8map!(:stable, :compose); end
    def utf8nfkd;  utf8map( :stable, :decompose, :compat); end
    def utf8nfkd!; utf8map!(:stable, :decompose, :compat); end
    def utf8nfkc;  utf8map( :stable, :compose, :compat); end
    def utf8nfkc!; utf8map!(:stable, :compose, :compat); end
    def utf8chars
      result = self.utf8map(:charbound).split("\377")
      result.shift if result.first == ""
      result
    end
    def char_ary
      # depecated, use String#utf8chars instead
      utf8chars
    end
  end

  module IntegerExtensions
    def utf8
      return Utf8Proc::utf8char(self)
    end
  end

end


class String
  include(Utf8Proc::StringExtensions)
end

class Integer
  include(Utf8Proc::IntegerExtensions)
end

