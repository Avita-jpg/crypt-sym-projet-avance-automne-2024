#!/usr/bin/python3
import threading
import queue
import time
import random
import argparse


def puissance_mod_n(a: int, e: int, n: int) -> int:
    return pow(a, e, n)  # a^e % n


def Alice(p,g,queueAlice,queueBob,queueCle):
   print(f"[Connu d'Alice, Bob et de potentiels espions(Eve)] --> p = {p} et g = {g}")
   a = random.randint(4,10000)
   print(f"\n[Connu d'Alice seulement] --> a = {a}")

   A = puissance_mod_n (g,a,p)
   print(f"\n[Connu d'Alice, Bob et de potentiels espions(Eve)] --> A (g^a[p]) = {A}")
   queueAlice.put(A)
   
   while queueBob.empty() : # tant que Bob n'ajoute pas B à sa file, on attend
      time.sleep(1) 
   B = queueBob.get()

   cle = puissance_mod_n (B,a,p)
   queueCle.put(cle)
   print(f"[Connu d'Alice et Bob seulement] --> cle = {cle}")
   


def Bob(p,g,queueAlice,queueBob):
   b = random.randint(4,10000)
   print(f"\n[Connu de Bob seulement] --> b = {b}")

   B = puissance_mod_n (g,b,p)
   print(f"\n[Connu d'Alice, Bob et de potentiels espions(Eve)] --> B (g^b[p]) = {B}")
   queueBob.put(B)

   while queueAlice.empty() : 
      time.sleep(1)
   A = queueAlice.get()

   cle = puissance_mod_n (A,b,p)
   print(f"[Connu d'Alice et Bob seulement] --> cle = {cle}")



def main() :
  
  parser = argparse.ArgumentParser(description="Génération de la clé de Diffie-Hellman.")
  parser.add_argument('-i', '--input', type=str, required=True, help='Fichier d\'entrée avec les paramètres p et g.') # le fichier d'entrée avec les arguments
  parser.add_argument('-o', '--output', type=str, required=True, help='Fichier de sortie pour la clé générée.') # le fichier de sortie qui aura la clé

  args = parser.parse_args() 


  with open(args.input, 'r') as file: # lecture fichier mis in input -i
      p = int(file.readline().strip())  # lire p dans la première ligne
      g = int(file.readline().strip())  # lire g dans la deuxième ligne

  print(f"p : {p}")
  print(f"g : {g}")

  queueAlice = queue.Queue()  # file d'attente pour A (g^a[p]) 
  queueBob = queue.Queue()  # file d'attente pour B (g^b[p]) 
  queueCle = queue.Queue() # file d'attente pour la clé finale

  t1=threading.Thread(target=Alice,args=(p,g,queueAlice,queueBob,queueCle,)) # création du thread de la fonction alice avec les arguments donnés
  t2=threading.Thread(target=Bob,args=(p,g,queueAlice,queueBob,))

  t1.start() # lancement du thread d'Alice
  t2.start()

  t1.join() # pour que le programme attente que les thread se terminent avant de poursuivre ici
  t2.join()

  cle = queueCle.get()
  with open(args.output, 'w') as fichier:
    fichier.write(str(cle)) # écriture de la clé dans le fichier output -o

  print("terminé!")


main()