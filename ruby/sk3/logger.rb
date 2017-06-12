require "event_queue"

module SK3

  LogRecordType = {
    :deficit     => "BufferChg ",
    :active_end  => "ActiveEnd ",
    :idle_end    => "IdleEnd   ",
    :ripple_end  => "RippleEnd ",
    :buffer_full => "BufferFull"
  }

  class Logger
    attr_accessor :event_queue

    def curtime
      @event_queue.curtime
    end

    def close
      fail "Must be subclassed"
    end

    def log_line(type, reporter, length, details=nil)
      return unless LogRecordType.has_key?(type)
      curtime = @event_queue.curtime
      line = "#{SK3.time_format(curtime)} (#{SK3.time_format(length)}): #{LogRecordType[type]} (#{reporter.name})#{details ? ' ' : ''}"
      case type
      when :deficit
        line << "deficit: #{SK3.quantity_format(reporter.buffer)}"
      when :active_end
        line << details.map { |s|
          "#{s.name}:#{s.count},#{SK3.quantity_format(s.low_water_mark)}"
        }.join(" ")
      when :ripple_end
        line << details.to_s
      end
      line
    end

    def log(*args)
      fail "Must be subclassed"
    end

  end
end

