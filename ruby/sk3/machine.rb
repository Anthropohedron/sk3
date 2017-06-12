
class Machine
  attr_reader :name, :tasks, :queue, :doing

  class Stat
    attr_reader :task, :low_water_mark, :count

    def initialize(_task, _count, _low_water_mark)
      @task = _task
      @count = _count
      @low_water_mark = _low_water_mark
    end

    def name
      @task.name
    end

    def <=>(other)
      name <=> other.name
    end

  end

  def initialize(event_queue, machine_name)
    @event_queue = event_queue
    @name = machine_name
    @tasks = {}
    @queue = []
    @doing = nil
    @event = method(:finish_task)
  end

  def init_sim
    start_next_task
    unless @doing
      @idle_start = @event_queue.curtime
    end
  end

  def add_tasks(*add)
    add.each { |t| add_task(t) }
  end

  def add_task(task)
    if task.machine && task.machine != self
      fail "Already assigned to another machine"
    end
    @tasks[task] = 0
    task.machine = self
  end

  def enqueue(task)
    fail "Can't queue a task on the wrong machine" unless @tasks[task]
    return if ((task == @doing) || @queue.include?(task))
    @queue.push task
    was_idle = !@doing
    start_next_task
    log_idle_end if was_idle
  end

  private

  def log_active_end
    return if @doing
    @idle_start = @event_queue.curtime
    length = 0
    statistics = @tasks.keys.inject([]) { |stats,task|
      count = @tasks[task]
      stats << Stat.new(task, count, task.low_water_mark)
      @tasks[task] = 0
      task.reset_low_water_mark!
      length += task.batch_time * count
      stats
    }.sort
    @event_queue.log(:active_end, self, length, statistics)
  end

  def log_idle_end
    return unless @doing && @idle_start
    length = @event_queue.curtime - @idle_start
    @idle_start = nil
    @event_queue.log(:idle_end, self, length)
  end

  def start_next_task
    return if @doing
    @doing = queue.shift
    if @doing
      @doing.start_batch
      @tasks[@doing] += 1
      @event_queue.add_event(@doing.batch_time) do
        finish_task
      end
    end
  end

  def finish_task
    return unless @doing
    task = @doing
    @doing = nil
    task.finish_batch
    start_next_task
    log_active_end
  end

end
