#!/usr/bin/env ruby

$LOAD_PATH << File.join(File.dirname(__FILE__), "sk3")

module SK3
  extend self
  TimeDecimals = 3
  QuantityDecimals = 3

  TimeFactor = 10 ** TimeDecimals
  QuantityFactor = 10 ** QuantityDecimals

  def time_format(time)
    sprintf("%d.%0#{TimeDecimals}d",
            time / TimeFactor, time % TimeFactor);
  end

  def to_internal_time(time)
    (time * TimeFactor).round
  end

  def quantity_format(quantity)
    sprintf("%d.%0#{QuantityDecimals}d",
            quantity / QuantityFactor, quantity % QuantityFactor);
  end

  def to_internal_quantity(quantity)
    (quantity * QuantityFactor).round
  end

  def internal_quantity_mul(q1, q2)
    (q1 * q2) / QuantityFactor
  end

  def get_positive_quantity(options, key, alt=nil)
    if Array === key
      keys = key.dup
      begin
        key = keys.shift
        return get_positive_quantity(options, key, alt)
      rescue
        if keys.empty?
          raise $!
        else
          retry
        end
      end
    end
    quantity = options[key.to_sym] || options[key.to_s]
    if quantity
      unless quantity > 0
        fail "Positive #{key} is required"
      end
    else
      if alt
        quantity = alt
      else
        fail "Positive #{key} is required"
      end
    end
    SK3.to_internal_quantity(quantity)
  end

  def get_positive_time(options, key, alt=nil)
    if Array === key
      keys = key.dup
      begin
        key = keys.shift
        return get_positive_time(options, key, alt)
      rescue
        if keys.empty?
          raise $!
        else
          retry
        end
      end
    end
    time = options[key.to_sym] || options[key.to_s]
    unless time
      if alt
        time = alt
      else
        fail "Positive #{key} is required"
      end
    else
      unless time > 0
        fail "Positive #{key} is required"
      end
    end
    SK3.to_internal_time(time)
  end

end

require "event_queue"
require "task"
require "machine"
require "demand"
require "config_reader"

#TODO
