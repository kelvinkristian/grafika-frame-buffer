import os

class CharGenerator():
    def __init__(self):
        self.chars = {}

    def load_chars(self):
        filenames = os.listdir()
        for filename in filenames:
            if len(filename) != 5 or filename[2:] != 'txt':
                continue

            with open(filename) as f:
                content = f.read()

            temp = content.split(' ')
            temp = list(map(lambda x: x.split(','), temp))
            temp = list(map(lambda x: (int(x[0]), int(x[1])), temp))

            self.chars[filename[0]] = temp

    def generate_coordinate(self, char, offset_x, offset_y):
        coordinates = self.chars[char]
        result = []
        for y, x in coordinates:
            result.append("{} {}".format(x + offset_x, y + offset_y))

        return result

    def generate_name(self, name, offset_y):
        offset_x = 0
        result = []
        for char in name:
            if char in self.chars:
                result += self.generate_coordinate(char, offset_x, offset_y)
            offset_x += 20
            
        return result

    def generate_names(self, names):
        offset_y = 0
        result = []
        for name in names:
            result += self.generate_name(name, offset_y)
            offset_y += 26

        return result


if __name__ == "__main__":
    char_gen = CharGenerator()
    char_gen.load_chars()
    result = char_gen.generate_names([
        "renjira",
        "gabriel",
        "naufal",
        "shinta",
        "priagung",
        "kelvin"
    ])
    with open('result.txt', 'w') as f:
        f.write('\n'.join(result) + '\n')
    
        
