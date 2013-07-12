require 'rubygems'
require 'chunky_png'

image = ChunkyPNG::Image.from_file(ARGV[0])
(0..(image.height-1)).each do |index|
p image.row(index)
end
puts "#############################################################"
bitmap = []
(0..(image.height-1)).each do |index|
  raw = image.row(index).inject([]) do |r,e|
    st = ''
    if e != 4294967295
      st << '1'
    else
      st << '0'
    end
    r << st
    r
  end
  raw.each_with_index do |x,i|
    if (i % 8 == 0)
      if i == 0
        raw[i] = "B#{raw[i]}"
      else
        raw[i] = ",B#{raw[i]}"
      end
    end
  end
  raw[raw.size - 1] = raw[raw.size - 1] + ','
  puts raw.join('')
end

