require 'rubygems'
require 'bundler'
include Rake::DSL

begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end
require 'rake'

require 'rake/extensiontask'
Rake::ExtensionTask.new('mac_address_book') do |ext|
  ext.lib_dir = File.join("lib", "mac_address_book")
end

require 'jeweler'
Jeweler::Tasks.new do |gem|
  gem.name = "mac_address_book"
  gem.homepage = "http://github.com/deardaniel/mac_address_book"
  gem.license = "MIT"
  gem.summary = %Q{A ruby interface to OS X's Address Book}
  gem.description = %Q{This is a simple wrapper around OS X's AddressBook framework's classes allowing access to the Address Book database from Ruby.}
  gem.email = "me@daniel.ie"
  gem.authors = ["Daniel Heffernan"]
  gem.extensions = FileList["ext/**/extconf.rb"]
  gem.add_development_dependency 'rake-compiler'
  gem.files = FileList['**/*']
end
Jeweler::RubygemsDotOrgTasks.new


require 'rake/testtask'
Rake::TestTask.new(:test) do |test|
  test.libs << 'lib' << 'test'
  test.pattern = 'test/**/test_*.rb'
  test.verbose = true
end

require 'rcov/rcovtask'
Rcov::RcovTask.new do |test|
  test.libs << 'test'
  test.pattern = 'test/**/test_*.rb'
  test.verbose = true
end

task :default => :test

require 'rdoc/task'
RDoc::Task.new do |rdoc|
  version = File.exist?('VERSION') ? File.read('VERSION') : ""

  rdoc.rdoc_dir = 'rdoc'
  rdoc.title = "mac_address_book #{version}"
  rdoc.rdoc_files.include('README*')
  rdoc.rdoc_files.include('lib/**/*.rb')
end
