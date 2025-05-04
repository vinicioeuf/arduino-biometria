import requests


url = "https://exultant-plum-mandolin.glitch.me/addacessos"

payload = {
    "nome": "Alvaro Victor",
    "email": "alvaro.victor@aluno.ifsertao-pe.edu.br",
    "idbiometria": 13,  # Corrigido aqui
    "foto": "https://link-da-foto.com/foto32.jpg",
    "tipo": "Saiu"

}


headers = {
    "Content-Type": "application/json"
}


response = requests.post(url, json=payload, headers=headers)


print("Status Code:", response.status_code)
print("Resposta:", response.text)