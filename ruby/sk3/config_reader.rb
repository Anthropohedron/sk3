require "sk3/ext/split_logger"
require "sk3/task"
require "sk3/machine"
require "sk3/demand"
require "sk3/commit_task"
require "sk3/commit_demand"
require "sk3/ext/communicating_task"
require "sk3/ext/communicating_demand"
require "rubygems"
require "yaml"

module SK3

  SK3 = self

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

    BaseLogConfig = { "dir" => "/tmp" }
    BaseVarConfig = {
      "task"    => "Task",
      "machine" => "Machine",
      "demand"  => "Demand"
    }

    module Util

      def resolve(path, top=SK3)
        begin
          parts = path.split('::')
          if parts[0] == ''
            top = Object
            parts.shift
          end
          parts.reduce(top) { |parent,name|
            parent.const_get(name)
          }
        rescue
          if top == Object
            fail "Not found: #{path}"
          else
            resolve(path, Object)
          end
        end
      end

      extend self

    end

    def read_config(config)
      cfg = YAML.load(config)
      logcfg = BaseLogConfig.merge(cfg["logger"] || {})
      varcfg = BaseVarConfig.merge(cfg["variants"] || {})
      @event_queue.logger = SplitLogger.new(logcfg["split"], logcfg["dir"])
      task_class    = Util.resolve(varcfg["task"])
      machine_class = Util.resolve(varcfg["machine"])
      demand_class  = Util.resolve(varcfg["demand"])
      cfg["tasks"].each do |name,options|
        batch_size = SK3.get_positive_quantity(options, :batch_size)
        batch_time = begin
                       SK3.get_positive_time(options, :batch_time)
                     rescue
                       batch_size * SK3.get_positive_time(options, :unit_time)
                     end
        buffer = -SK3.get_positive_quantity(options, [ :deficit, :debit ], 0)
        @tasks[name] = task_class.new(@event_queue, name,
                                      batch_time, batch_size, buffer)
      end
      cfg["machines"].each do |name,tasklist|
        machine = machine_class.new(@event_queue, name)
        @machines[name] = machine
        args = tasklist.map { |n| n.to_s }.sort.uniq.map { |task_name|
          task = @tasks[task_name]
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
      end
      cfg["demands"].each do |options|
        name = options[:supplier] || options["supplier"]
        source = @tasks[name]
        fail "No such task: #{name.inspect}" unless source
        quantity = SK3.get_positive_quantity(options, :quantity)
        sink_name = options[:destination] || options["destination"]
        if sink_name && sink_name != :external
          sink = @tasks[sink_name.to_s]
          fail "No such task: #{sink_name.inspect}" unless sink
          sink.add_prereq(source, quantity)
        else
          if @demands[name]
            fail "Duplicate demand for #{name.inspect}"
          end
          interval = SK3.get_positive_time(options, :interval)
          offset = SK3.get_positive_time(options, :offset, 0)
          if offset > interval
            fail "Offset (#{offset}) must be less than or equal to interval (#{interval})"
          end
          @demands[name] =
            demand_class.new(@event_queue, source, quantity, interval, offset)
        end
      end
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

