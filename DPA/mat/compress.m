function [ traces_c ] = compress( traces, interval )
%% Compress Trace Matrix by using sum of 1 clock cycle
nTraces = size(traces,1);

% reader_clock = 4.8e6; % Cherry Card Reader Clock
% samplesPerSecond = 250e6;
% interval = floor(samplesPerSecond/reader_clock);

if nargin < 2
    interval = 26;
    %interval = 52; % 1/clock
    %interval = 77; % 1/2clock
end

%tracesLength = size(traces,2);
%compresser = ones(interval, 1);
%repetitions = floor(tracesLength/interval);

%compressMatrix = [repmat([compresser;zeros(interval*repetitions,1)],repetitions-1,1); compresser];
%compressMatrix = reshape(compressMatrix,[],repetitions); 
%traces_c = traces * compressMatrix;

T = 0:size(traces,2)-1;
ix = 1+floor((T-T(1))/interval);

traces_c = zeros(nTraces, ix(end));

for row = 1:nTraces
    display(['compressing ', num2str(row), '/', num2str(nTraces)]);
    traces_c(row,:) = accumarray(ix(:),traces(row,:));
end

traces_c = traces_c / interval;

display('Finished compress')

end

