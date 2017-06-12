require 'sk3/task'

module SK3

  class CommunicatingTask < Task
    attr_reader :commitment

    def initialize(*args)
      super(*args)
      @commitment = 0
    end

    def take_from_buffer(quantity, demand)
      @commitment -= quantity
      if @commitment < 0
        STDERR.puts "buffer = #{@buffer}, commitment = #{@commitment}, quantity = #{quantity}"
        fail "commitment went negative!"
      end
      super(quantity, demand)
    end

    def communicate_commitment(deficit)
      if deficit < 0
        STDERR.puts "buffer = #{@buffer}, commitment = #{@commitment}, deficit = #{deficit}"
        fail "deficit went negative?"
      end
      @commitment += deficit
      @prereqs.each do |task,quantity|
        new_commitment = SK3.internal_quantity_mul(quantity, deficit)
        task.communicate_commitment(new_commitment)
      end
      @machine.enqueue(self) if should_enqueue?
    end

    def should_enqueue?
      (@commitment - @buffer) > 0
    end

  end

end

