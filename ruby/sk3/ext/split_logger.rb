require "sk3/logger"
require "fileutils"

module SK3
  class SplitLogger < SK3::Logger

    def initialize(count_limit, prefix="/tmp")
      FileUtils.mkdir_p prefix
      @streams = Hash.new { |h,k|
        fname = File.join(prefix, k) + ".log"
        h[k] = File.open(fname, "w")
      }
      @counts = count_limit && Hash.new(count_limit)
    end

    def close
      @streams.each { |k,f| f.close }
      @streams.clear
      @counts.clear if @counts
      nil
    end

    def log(type, reporter, *args)
      line = log_line(type, reporter, *args)
      return unless line
      key = reporter.name
      @streams[key].puts line
      if @counts
        @counts[key] -= 1
        if @counts.all? { |k,v| v <= 0 }
          throw :stop_run, [ curtime, "Sufficient log lines" ]
        end
      end
    end

  end
end

