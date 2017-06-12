
module SK3

  class CommitTask < Task
    def initialize(*args)
      super(*args)
      @commitment = 0
    end

    def take_from_buffer(quantity, demand, external = false)
      super(quantity, demand)
      if external
        cascade_commitment(quantity, false)
      else
        @commitment -= quantity
      end
    end

    def cascade_commitment(quantity, commit_self = true)
      @commitment += quantity if commit_self
      @prereqs.each do |task,mult|
        task.cascade_commitment(quantity * mult)
      end
    end

    def should_enqueue?
      super() || (@commitment > 0)
    end
  end

end

