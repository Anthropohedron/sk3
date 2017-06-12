tasks do
  t1 :batch_time => 1.0, :batch_size => 1
  t2 :batch_time => 0.9, :batch_size => 1
end

machines do
  m1 :t1
  m2 :t2
end

demands do
  t1 :quantity => 1, :to => :t2
  t2 :quantity => 1, :interval => 1.1
end

