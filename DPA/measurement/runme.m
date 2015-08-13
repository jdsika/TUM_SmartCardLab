clc
clear

%% Specify measurement parameters
nTraces = 500;              % Number of traces measured
sampleRate = 250e6;         % Minimum sample rate used [S/s]
sampleTime =  500e-6;       % Total sampled time per measurement [s]
outDirectory = 'traces-0';  % Output directory for the measurements

%% Start measurement
traces = trace_measurement(nTraces,sampleRate,sampleTime,outDirectory);

reader_clock = 4.8e6; % 4.8 MHz (Cherry Cardreader)
t_time  = linspace(0, sampleTime, sampleRate * sampleTime);
t_clock = linspace(0, reader_clock * sampleTime, sampleRate * sampleTime);

ciphertext = importdata([outDirectory, filesep, 'ciphertext.csv']);
plaintext = importdata([outDirectory, filesep, 'plaintext.csv']);

save([outDirectory, filesep, 'measurement']);
