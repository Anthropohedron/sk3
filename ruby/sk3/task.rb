
module SK3

  class Task
    attr_reader :name, :batch_time, :batch_size, :buffer, :low_water_mark
    attr_accessor :machine

    def initialize(event_queue, _name,
                   _batch_time, _batch_size, _buffer = 0)
      @event_queue = event_queue
      @name        = _name
      @batch_time  = _batch_time
      @batch_size  = _batch_size
      @buffer      = _buffer
      @prereqs = {}
      @demands = []
    end

    def init_sim
      fail "Task #{@name} isn't assigned to a machine" unless @machine
      if @buffer < 0
        @start_time = @event_queue.curtime
        @machine.enqueue(self)
      end
      reset_low_water_mark!
    end

    def reset_low_water_mark!
      @low_water_mark = @buffer
    end

    def add_prereq(task, quantity)
      if @prereqs[task]
        fail "Duplicate requirement: #{name} already requires #{task.name}"
      end
      @prereqs[task] = quantity
    end

    def start_batch
      @prereqs.each do |task,quantity|
        task.take_from_buffer(quantity, @demands.first)
      end
    end

    def finish_batch
      produced = @batch_size
      self.buffer += produced
      while (produced > 0) && @demands.first
        produced = @demands.first.fulfilling(self, produced)
        if produced
          @demands.shift
        else
          produced = 0
        end
      end
      if @buffer < 0
        @machine.enqueue(self)
      else
        log_filled_buffer
      end
    end

    def take_from_buffer(quantity, demand)
      if (@buffer >= 0) && (@buffer < quantity)
        @start_time = @event_queue.curtime
      end
      self.buffer -= quantity
      @low_water_mark = @buffer if @buffer < @low_water_mark
      @machine.enqueue(self) if should_enqueue?
      return if @buffer >= 0
      if demand
        if quantity < -@buffer
          demand.taking(self, -@buffer)
        else
          demand.taking(self, quantity)
        end
        @demands.push demand unless @demands.include? demand
      end
    end

    # hook for subclass
    def should_enqueue?
      @buffer < 0
    end

    def hash
      @name.hash
    end

    protected

    def buffer=(quantity)
      @buffer = quantity
      @event_queue.log(:deficit, self, 0)
    end

    private

    def log_filled_buffer
      length = @event_queue.curtime - @start_time
      @event_queue.log(:buffer_full, self, length)
    end

  end

end
