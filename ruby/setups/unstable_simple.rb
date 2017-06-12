
require "sk3/ext/split_logger"
logger SK3::SplitLogger.new(nil, "/tmp/unstableSK3")

tasks {
  t1 :batch_time => 7.2, :batch_size => 9, :deficit => 100
  t2 :batch_time => 0.1, :batch_size => 1
  t3 :batch_time => 7.2, :batch_size => 9
  t4 :batch_time => 0.1, :batch_size => 1
}

machines {
  m1 :t1, :t4
  m2 :t2, :t3
}

demands {
  t1 :quantity => 1, :to => :t2
  t2 :quantity => 1, :interval => 1
  t3 :quantity => 1, :to => :t4
  t4 :quantity => 1, :interval => 1
} 

