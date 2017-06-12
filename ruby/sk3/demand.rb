module SK3

  class Demand

    class Token
      def initialize(event_queue, demand, serial)
        @event_queue = event_queue
        @start_time = event_queue.curtime
        @demand = demand
        @serial = serial
        @products = Hash.new(0)
      end

      def name
        @demand.name
      end

      def taking(task, quantity)
        @products[task] -= quantity
      end

      def fulfilling(task, quantity)
        value = @products[task] + quantity
        if value >= 0
          @products.delete task
          if @products.empty?
            log_demand_complete
          end
          value
        else
          @products[task] = value
          nil
        end
      end

      def complete?
        @products.empty?
      end

      private

      def log_demand_complete
        length = @event_queue.curtime - @start_time
        @event_queue.log(:ripple_end, self, length, @serial)
      end
    end

    attr_reader :task, :interval, :quantity

    def initialize(event_queue, task, quantity, interval, offset)
      @event_queue = event_queue
      @task = task
      @quantity = quantity
      @interval = interval
      @offset = offset
      @counter = 0
      @event = method(:run_demand)
    end

    def init_sim
      @event_queue.add_event(@interval - @offset, &@event)
    end

    def run_demand
      @event_queue.add_event(interval, &@event)
      token = Token.new(@event_queue, self, @counter += 1)
      perform_demand(@quantity, token)
    end

    # hook for subclass
    def perform_demand(quant, token)
      @task.take_from_buffer(quant, token)
    end

    def name
      @task.name
    end

  end

end

