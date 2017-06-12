require 'sk3/demand'

module SK3

  class CommunicatingDemand < Demand

    def perform_demand(quant, token)
      @task.communicate_commitment(quant)
      super(quant, token)
    end

  end

end

