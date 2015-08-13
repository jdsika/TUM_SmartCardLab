function smartcard_measurement( nTraces, sampleRate, sampleTime, outDirectory, measurementNumber )
%% Wrapper Function for trace_measurement (replacement for runme)
% nTraces = 500;              % Number of traces measured
% sampleRate = 250e6;         % Minimum sample rate used [S/s]
% sampleTime =  500e-6;       % Total sampled time per measurement [s]
% outDirectory = 'traces-0';  % Output directory for the measurements

dataDirectory = [outDirectory, filesep, 'data-', num2str(measurementNumber)];
matFilename   = [outDirectory, filesep, 'measurement-', num2str(measurementNumber)];

traces = trace_measurement(nTraces,sampleRate,sampleTime,dataDirectory);

reader_clock = 4.8e6; % 4.8 MHz (Cherry Cardreader)
t_time  = linspace(0, sampleTime, sampleRate * sampleTime);
t_clock = linspace(0, reader_clock * sampleTime, sampleRate * sampleTime);

ciphertext = importdata([dataDirectory, filesep, 'ciphertext.csv']);
plaintext  = importdata([dataDirectory, filesep, 'plaintext.csv']);

save(matFilename, 'traces', 't_time', 't_clock', 'ciphertext', 'plaintext');

end

