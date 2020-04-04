import random


def gen():
    n = 200
    s = [chr(random.randrange(0, 9) + ord('0')) for i in range(n)]
    return ''.join(s)


print(gen())
print(gen())

