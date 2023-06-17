FROM python:3.11-alpine

COPY ./requirements.txt /app/requirements.txt

WORKDIR /app

RUN pip install -r requirements.txt

# copy app source files to workdir
COPY . /app

# runtime
ENTRYPOINT [ "python" ]