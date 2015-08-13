clear

folder = '~/Desktop/my Desktop is clean/clone-traces/masking/clone-masking-in_out-1_5k-1/measurement-';
directory = dir([folder, '*.mat']);
numFiles = length(directory(not([directory.isdir])));

tracesList = '';
ciphertextList = '';
plaintextList = '';

for k = 0:numFiles-1
    currentFile = [folder, num2str(k)];
    display(['Loading ', currentFile, ' ...']);

    load([currentFile, '.mat']);
    traces = compress(traces, 26);

    curTrace = ['traces_',num2str(k)];
    eval([curTrace,'=traces;']);
    curCipher = ['ciphertext_',num2str(k)];
    eval([curCipher,'=ciphertext;']);
    curPlain = ['plaintext_',num2str(k)];
    eval([curPlain,'=plaintext;']);
    
    tracesList = [tracesList, ',', curTrace];
    ciphertextList = [ciphertextList, ',', curCipher];
    plaintextList = [plaintextList, ',', curPlain];
    
    clear currentFile ciphertext plaintext traces cur*
end

traces = eval(['vertcat(', tracesList(2:end), ');']);
ciphertext = eval(['vertcat(', ciphertextList(2:end), ');']);
plaintext = eval(['vertcat(', plaintextList(2:end), ');']);

display('Finished combine');

clear k folder traces_* ciphertext_* plaintext_* *List