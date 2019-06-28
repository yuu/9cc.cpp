FROM debian:buster-20190506

RUN apt update && apt install -y build-essential cmake gdb
CMD ["tail", "-f"]
