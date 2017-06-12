require "rubygems"
require "blankslate"

module SK3

  class EvalObject < BlankSlate
    def initialize(&block)
      meta = (class << self; self; end)
      meta.send(:define_method, :method_missing, &block)
    end
  end

  class EvalReader

    def initialize(event_queue, tasks, machines, demands)
      @event_queue = event_queue
      @tasks = tasks
      @machines = machines
      @demands = demands
      @task_class = Task
      @machine_class = Machine
      @demand_class = Demand
    end

    def logger(_logger)
      @event_queue.logger = _logger
    end

    def task_class(klass)
      @task_class = klass
    end

    def machine_class(klass)
      @machine_class = klass
    end

    def demand_class(klass)
      @demand_class = klass
    end

    def tasks(&block)
      event_queue = @event_queue
      task_map = @tasks
      task_class = @task_class
      EvalObject.new { |name,options|
        name = name.to_s
        fail "Task name collision: #{name.inspect}" if task_map[name]
        batch_size = SK3.get_positive_quantity(options, :batch_size)
        batch_time = begin
                       SK3.get_positive_time(options, :batch_time)
                     rescue
                       batch_size * SK3.get_positive_time(options, :unit_time)
                     end
        buffer = -SK3.get_positive_quantity(options, [ :deficit, :debit ], 0)
        task_map[name] = task_class.new(event_queue, name,
                                  batch_time, batch_size, buffer)
      }.instance_eval(&block)
      nil
    end

    def machines(&block)
      event_queue = @event_queue
      task_map = @tasks
      machine_map = @machines
      machine_class = @machine_class
      EvalObject.new { |name,*args|
        name = name.to_s
        machine = (machine_map[name] ||= machine_class.new(event_queue, name))
        args = args.map { |n| n.to_s }.sort.uniq.map { |task_name|
          task = task_map[task_name]
          fail "Unknown task: #{task_name.inspect}" unless task
          if task.machine
            if task.machine == machine
              # already have it
              nil
            else
              fail "Task #{task_name.inspect} is assigned to machine #{task.machine.name.inspect} and cannot be assigned to machine #{name.inspect}"
            end
          else
            # all is well
            task
          end
        }.compact
        machine.add_tasks(*args) unless args.empty?
      }.instance_eval(&block)
      nil
    end

    def demands(&block)
      event_queue = @event_queue
      task_map = @tasks
      demand_map = @demands
      demand_class = @demand_class
      EvalObject.new { |source_name,options|
        source_name = source_name.to_s
        source = task_map[source_name]
        fail "No such task: #{source_name.inspect}" unless source
        quantity = SK3.get_positive_quantity(options, :quantity)
        sink_name = options[:to]
        if sink_name && sink_name != :external
          sink = task_map[sink_name.to_s]
          fail "No such task: #{sink_name.inspect}" unless sink
          sink.add_prereq(source, quantity)
        else
          if demand_map[source_name]
            fail "Duplicate demand for #{source_name.inspect}"
          end
          interval = SK3.get_positive_time(options, :interval)
          offset = SK3.get_positive_time(options, :offset, 0)
          if offset > interval
            fail "Offset (#{offset}) must be less than or equal to interval (#{interval})"
          end
          demand_map[source_name] =
            demand_class.new(event_queue, source, quantity, interval, offset)
        end
      }.instance_eval(&block)
    end

  end

  class ConfigReader
    attr_reader :event_queue, :tasks, :machines, :demands

    def initialize(queue_or_timelimit=nil)
      case queue_or_timelimit
      when EventQueue
        @event_queue = queue_or_timelimit
      when Numeric
        @event_queue = EventQueue.new(SK3.to_internal_time(queue_or_timelimit))
      else
        @event_queue = EventQueue.new
      end
      @tasks    = {}
      @machines = {}
      @demands  = {}
    end

    def read_config(config)
      EvalReader.new(@event_queue,
                     @tasks, @machines, @demands).instance_eval(config)
    end

    def init_and_run(duration=nil)
      unless @init_once
        @init_once = true
        @tasks.each do |name,task|
          task.init_sim
        end
        @demands.each do |name,demand|
          demand.init_sim
        end
        @machines.each do |name,machine|
          machine.init_sim
        end
      end
      final_time,msg = catch :stop_run do
        @event_queue.run(duration) do |time|
          if time === 0
            @tasks.each do |name,task|
              @event_queue.log(:deficit, task, 0)
            end
          end
        end
      end
      puts "********** Ended at #{final_time}:"
      puts "**********   #{msg}"
      @event_queue.close
    end

  end

end

