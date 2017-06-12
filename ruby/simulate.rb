#!/usr/bin/env ruby

$LOAD_PATH << File.dirname(__FILE__)
require "sk3"

time_limit = ARGV[0] && (Integer(ARGV.shift) rescue nil)

unless time_limit
  STDERR.puts "Usage: #{$PROGRAM_NAME} <time limit> [config...]"
  exit 1
end

time_limit = SK3.to_internal_time(time_limit)

reader = SK3::ConfigReader.new
config = ARGF.read
reader.read_config(config)
reader.init_and_run(time_limit)
