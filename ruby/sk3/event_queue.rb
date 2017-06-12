require "rubygems"
require "algorithms"

class Containers::Heap
  def top_priority
    @next && @next.key
  end
end

module SK3

  class EventQueue
    attr_reader :curtime
    attr_accessor :logger

    def initialize(endtime = nil)
      @endtime = endtime
      @queue = Containers::Heap.new { |x,y| x < y }
      @curtime = 0
    end

    def add_event(delay, &block)
      fail "Delay (#{delay}) must be positive" unless delay > 0
      @queue.push(@curtime + delay, block)
    end

    def run(duration=nil)
      unless logger
        require "ext/simple_logger"
        self.logger = SimpleLogger.new()
      end
      should_yield = block_given?
      logger.event_queue = self
      @endtime = @curtime + duration if duration
      yield @curtime if should_yield
      while (time = @queue.top_priority)
        if @endtime && (time > @endtime)
          return [ @curtime,  "Finished" ]
        end
        @curtime = time
        @queue.pop.call()
        yield @curtime if should_yield
      end
      [ @curtime,  "Ran out of events?" ]
      nil
    end

    def close
      logger.close
    end

    def log(type, *args)
      return unless LogRecordType.has_key?(type)
      logger.log(type, *args)
    end

  end

end

