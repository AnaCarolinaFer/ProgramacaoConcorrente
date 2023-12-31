#exemplo de uso de pool de processos em Python

from multiprocessing.pool import Pool
import time
import sys

#funcao que sera executada de forma concorrente
def task(n):
    if (n<=1): 
        return 0
    if (n==2): 
        return 1
    if (n%2==0): 
        return 0
    for i in range(3, int((n**1/2)+1), 2):
        if(n%i==0): 
            return 0
    return 1

if __name__ == '__main__':
    start = time.time()
    pool = Pool() #por default, cria um processo distinto para cada processador da maquina

    numbers = list(range(int(sys.argv[1])))
    #map aceita uma funcao e um objeto iteravel, o pool pega cada valor do objeto iteravel e passa para um processo disponivel no poll que aplica a funcao sobre esse valor
    results = pool.map(task, numbers)
    print(sum(results))
    end = time.time()
    print('work took {} seconds'.format(end - start))