import time

class Fib:
    def get(self, n):
        if (n < 2):
            return n
        return self.get(n - 2) + self.get(n - 1)

start = time.process_time()
fib = Fib()

for i in range(0, 100):
    print (fib.get(28))

print (time.process_time() - start)