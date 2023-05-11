input_file = open('input.txt')
output_file = open('output.txt', 'w')


for line in input_file:
    line = line.strip('\r\n')
    output_file.write('"' + line + '\\n"' + '\n')