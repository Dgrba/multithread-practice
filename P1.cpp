#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

using namespace std;

constexpr int SHARED_MEM_SIZE = 65536;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: ./p1 <file_name> <word_to_find>" << endl;
        return 1;
    }

    string fileName = argv[1];
    string word = argv[2];
    transform(word.begin(), word.end(), word.begin(), ::tolower);

    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    int fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        cerr << "Error creating shared memory." << endl;
        return 1;
    }

    if (ftruncate(fd, SHARED_MEM_SIZE) == -1) {
        cerr << "Error setting shared memory size." << endl;
        close(fd);
        return 1;
    }

    void *shared_memory = mmap(0, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        cerr << "Error mapping shared memory." << endl;
        close(fd);
        return 1;
    }

    strncpy(static_cast<char *>(shared_memory), content.c_str(), SHARED_MEM_SIZE - 1);

    wait(nullptr);

    cout << "Lines containing the target word:" << endl;
    cout << static_cast<char *>(shared_memory) << endl;

    munmap(shared_memory, SHARED_MEM_SIZE);
    close(fd);
    shm_unlink("/shared_memory");

    return 0;
}
