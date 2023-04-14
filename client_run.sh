# Compile the client code
gcc -o client client.c

# Run the 5 clients 
for i in {1..5}; do
  ./client 127.0.0.1 8080 &
done