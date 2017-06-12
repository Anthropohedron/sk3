
module SK3

  class CommitDemand < Demand
    def perform_demand(quant, token)
      @task.take_from_buffer(quant, token, true)
    end
  end

end

