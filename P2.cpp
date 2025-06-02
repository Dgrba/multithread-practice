#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

using namespace std;

constexpr int SHARED_MEM_SIZE = 65536;
constexpr int NUM_THREADS = 4;

struct ThreadData {
    int thread_id;
    string content;
    vector<string> result;
};

string word;

void *mapper(void *arg) {
    ThreadData *data = static_cast<ThreadData *>(arg);
    istringstream stream(data->content);
    string line;

    while (getline(stream, line)) {
        string lowercase_line = line;
        transform(line.begin(), line.end(), lowercase_line.begin(), ::tolower);

        size_t pos = lowercase_line.find(word);
        if (pos != string::npos && (pos == 0 || !isalnum(lowercase_line[pos - 1])) &&
            (pos + word.length() == lowercase_line.length() || !isalnum(lowercase_line[pos + word.length()]))) {
            data->result.push_back(line);
        }
    }

    return nullptr;
}

void reducer(ThreadData *data, void *shared_memory) {
    string result;
    for (int i = 0; i < NUM_THREADS; ++i) {
        for (const auto &line : data[i].result) {
            result += line + "\n";
        }
    }
    strncpy(static_cast<char *>(shared_memory), result.c_str(), SHARED_MEM_SIZE - 1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: ./p2 <file_name> <word_to_find>" << endl;
        return 1;
    }

    word = argv[2];
    transform(word.begin(), word.end(), word.begin(), ::tolower);

    int fd = shm_open("/shared_memory", O_RDWR, 0666);
    if (fd == -1) {
        cerr << "Error opening shared memory." << endl;
        return 1;
    }

    void *shared_memory = mmap(0, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        cerr << "Error mapping shared memory." << endl;
        close(fd);
        return 1;
    }

    string content(static_cast<char *>(shared_memory));

    vector<string> parts(NUM_THREADS);
    istringstream stream(content);
    string line;
    int part = 0;
    while (getline(stream, line)) {
        parts[part] += line + "\n";
        part = (part + 1) % NUM_THREADS;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        data[i].thread_id = i;
        data[i].content = parts[i];
        pthread_create(&threads[i], nullptr, mapper, &data[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    reducer(data, shared_memory);

    munmap(shared_memory, SHARED_MEM_SIZE);
    close(fd);

    return 0;
}
