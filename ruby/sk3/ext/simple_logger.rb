require "sk3/logger"

module SK3
  class SimpleLogger < SK3::Logger
    attr_accessor :log_stream

    def initialize(time_limit = 1_000_000, stream = STDOUT)
      @time_limit = time_limit
      @pause_at = time_limit || Float::MAX
      @log_stream = stream
    end

    def close
      log_stream.close
    end

    def log(*args)
      line = log_line(*args)
      return unless line
      @log_stream.puts line
      if curtime >= @pause_at
        STDERR.print "Continue (Y/n)? "
        if /^[nNqQxX]/ === STDIN.readline
          throw :stop_run, [ curtime, "user cancelled" ]
        end
        @pause_at += @time_limit
      end
    end

  end
end

