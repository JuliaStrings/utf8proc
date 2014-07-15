require 'rubygems'
SPEC = Gem::Specification.new do |s|
  s.name = 'utf8proc'
  s.version = '1.1.6'
  s.author = 'Public Software Group e. V., Berlin, Germany'
  s.homepage = 'http://www.public-software-group.org/utf8proc'
  s.summary = 'UTF-8 Unicode string processing'
  s.files = ['LICENSE', 'lib/utf8proc.rb', 'ext/utf8proc_native.c']
  s.require_path = 'lib/'
  s.extensions = ['ext/extconf.rb']
  s.has_rdoc = false
end
