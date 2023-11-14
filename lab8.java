/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 8 */
/* Codigo: Criando um pool de threads em Java */

import java.util.LinkedList;

//-------------------------------------------------------------------------------
//Classe que define um pool de threads 
//!!! Documentar essa classe !!!
class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads;
    private final LinkedList<Runnable> queue;
    private boolean shutdown;

    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i = 0; i < nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        }
    }

    public void execute(Runnable r) {
        synchronized (queue) {
            if (this.shutdown)
                return;
            queue.addLast(r);
            queue.notify();
        }
    }

    public void shutdown() {
        synchronized (queue) {
            this.shutdown = true;
            queue.notifyAll();
        }
        for (int i = 0; i < nThreads; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                return;
            }
        }
    }

    private class MyPoolThreads extends Thread {
        public void run() {
            Runnable r;
            while (true) {
                synchronized (queue) {
                    while (queue.isEmpty() && (!shutdown)) {
                        try {
                            queue.wait();
                        } catch (InterruptedException ignored) {
                        }
                    }
                    if (queue.isEmpty() && shutdown)
                        return;
                    r = (Runnable) queue.removeFirst();
                }
                try {
                    r.run();
                } catch (RuntimeException e) {
                }
            }
        }
    }
}
// -------------------------------------------------------------------------------

// --PASSO 1: cria uma classe que implementa a interface Runnable
class PrimoCheck implements Runnable {
    long num;
    static int primoCont = 0;

    public PrimoCheck(long n) {
        num = n;
    }

    public void run() {
        if (ehPrimo(num)) {
            synchronized (this) {
                primoCont++;
            }
        }
    }

    private boolean ehPrimo(long n) {
        if (n <= 1)
            return false;
        if (n == 2)
            return true;
        if (n % 2 == 0)
            return false;
        for (int i = 3; i <= Math.sqrt(n); i += 2) {
            if (n % i == 0)
                return false;
        }
        return true;
    }
}

// Classe da aplicação (método main)
class MyPool {
    private static final int NTHREADS = 10;

    public static void main(String[] args) {
        // --PASSO 2: cria o pool de threads
        FilaTarefas pool = new FilaTarefas(NTHREADS);

        // --PASSO 3: dispara a execução dos objetos runnable usando o pool de threads
        for (int i = 0; i < 100; i++) {
            PrimoCheck primoCheck = new PrimoCheck(i);
            pool.execute(primoCheck);
        }

        // --PASSO 4: esperar pelo termino das threads
        pool.shutdown();
        System.out.println("De 0 a 100 existem " + PrimoCheck.primoCont + " primos");
        System.out.println("Terminou");
    }
}