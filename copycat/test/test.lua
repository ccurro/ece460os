-- Testing various buffer sizes 
require 'sys'
require 'gnuplot'

os.execute("head -c 1M /dev/urandom | hexdump > f1")
os.execute("head -c 1M /dev/urandom | hexdump > f2")
os.execute("head -c 1M /dev/urandom | hexdump > f3")

bufferSize = torch.pow(2,torch.range(0,18))
times = torch.zeros(bufferSize:size())
n = 20

for j = 1,n do
	for i = 1,bufferSize:numel() do
		sys.tic()
		local command = string.format("../src/copycat -b %d -o /tmp/buffer%d f1 f2 f3", bufferSize[i], bufferSize[i])
		os.execute(command)
		local diff = sys.toc()
		times[i] = times[i] + diff/n
	end
end

throughput = torch.cdiv(torch.Tensor(times:size()):fill(3),times)

print(throughput)
gnuplot.pdffigure("test.pdf")
gnuplot.plot(throughput,'-')
gnuplot.xlabel('2^N Buffer Size (Bytes)')
gnuplot.ylabel('Throughput (MB/s)')
gnuplot.plotflush()
