import telebot
from telebot.types import Message, InlineKeyboardButton, InlineKeyboardMarkup
from requests_toolbelt.multipart.encoder import MultipartEncoder
#from markups import main_menu_markup
import requests

api_key='7707206046:AAHaRoz0RWsKRXMxO9yXKFkMUCDS2uVzkJ8' 
bot = telebot.TeleBot(api_key)

users = [] #login, pass, session_id
def get_path_to_handler(handler:str):
    return 'http://localhost:8080/v1/'+handler

def is_login(login:str) -> bool:
    logins = [user['login'] for user in users]
    if(login in logins):
        return True
    return False

def get_user_info(username):
    return [user for user in users if user['login'] == username][0]

def create_markup(labels:list):
    markup = telebot.types.ReplyKeyboardMarkup(resize_keyboard=True, one_time_keyboard=True)
    for label in labels:
        but = telebot.types.KeyboardButton(label)
        markup.row(but)
    return markup

main_menu_commands = ['➕ Создать очередь', '📱 Мои очереди' ]
main_menu_markup = create_markup(main_menu_commands)


    
    


def start_bot():
    bot.infinity_polling()

@bot.message_handler(func=lambda _: True)
def echo_all(message:Message):
    if(not is_login(message.from_user.username)):
        bot.send_message(message.chat.id, 'Привет! Отправь мне свой пароль')
        bot.register_next_step_handler(message, login)
        return
    
    user_info = get_user_info(message.from_user.username)
    answer = f'Привет! {user_info['login']}! Что будем делать?'
    bot.send_message(message.chat.id, answer, reply_markup=main_menu_markup)
    bot.register_next_step_handler(message, main_menu)


def main_menu(message:Message):
    if(message.text not in main_menu_commands):
        bot.send_message(message.chat.id, 'Я тебя не понял, давай попробуем ещё раз', reply_markup=main_menu_markup)
        bot.register_next_step_handler(message, main_menu)

    if(message.text == main_menu_commands[0]):      # Create queue
        bot.send_message(message.chat.id, 'Как назовём новую очередь?\nНазвание должно содержать только латинские буквы и цифры. Для отмены - напишите "Отменить"')
        bot.register_next_step_handler(message, create_queue)
    elif (message.text == main_menu_commands[1]):   #Get my queues
        session_id = get_user_info(message.from_user.username)['session_id']
        queues = Handlers.get_all_queues(session_id)
        if(len(queues) == 0):
            bot.send_message(message.chat.id, 'У вас пока нет доступа ни к одной очереди, возвращаемся в главное меню', reply_markup= main_menu_markup)
            bot.register_next_step_handler(message, main_menu)
            return
        queues_markup = create_markup(make_uniq([queue['name'] for queue in queues]))
        queues_markup.add('⬅️ Назад')
        bot.send_message(message.chat.id, 'Чтобы посмотреть информацию о конкретной очереди - нажмите на неё', reply_markup= queues_markup)
        bot.register_next_step_handler(message, queue_info)

def login(message:Message):
    if(not message.text.isalnum() or len(message.text) < 5 ):
        bot.send_message(message.chat.id, 'Пароль должен быть из цифр и латинских букв. (от 6 символов) ')
        bot.register_next_step_handler(message, login)
        return
    
    user_info = Handlers.login(message.from_user.username, message.text)
    bot.delete_message(message.chat.id, message.id)
    if(user_info == None):
        answer = f'Не тот пароль, попробуй ещё!'
        bot.send_message(message.chat.id, answer)
        bot.register_next_step_handler(message, login)
        return
    users.append(user_info)
    answer = f'Привет, {user_info['login']} - ты в системе. Что будем делать?'
    bot.send_message(message.chat.id, answer, reply_markup=main_menu_markup)
    bot.register_next_step_handler(message, main_menu)

def create_queue(message:Message):
    if(message.text is None):
        bot.send_message(message.chat.id,'Я не нашёл текст, попробуем ещё раз.')
        bot.register_next_step_handler(message, create_queue)
        return
    if message.text == 'Отменить':
        bot.send_message(message.chat.id, "Возвращаемся в главное меню", reply_markup=main_menu_markup)
        bot.send_message(message.chat.id, "Что будем делать?", reply_markup=main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    if(not message.text.strip().isalnum()):
        bot.send_message(message.chat.id,'Название должно содержать только латинские буквы и цифры')
        bot.register_next_step_handler(message, create_queue)
        return
    session_id = get_user_info(message.from_user.username)['session_id']
    queue = Handlers.create_queue(message.text.strip(), session_id)
    if(queue is None):
        answer = f'Не удалось создать очередь, попробуйте другое название.\nВозвращаемся в главное меню'
    else:
        answer = f'Очередь {queue['name']} успешно создана, возвращаемся в главное меню'

    bot.send_message(message.chat.id, answer, reply_markup=main_menu_markup)
    bot.register_next_step_handler(message, main_menu)

def queue_info(message:Message):
    session_id = get_user_info(message.from_user.username)['session_id']
    possible_queues = Handlers.get_all_queues(session_id)


    queues_names = [queue['name'] for queue in possible_queues]
    queues_names.append('⬅️ Назад')
    queues_names = make_uniq(queues_names)
    
    queues_markup = create_markup(queues_names)

    if message.text is None or message.text not in queues_names:
        bot.send_message(message.chat.id, 'Я вас не понял', reply_markup= queues_markup)
        bot.register_next_step_handler(message, queue_info)
        return

    if message.text == queues_names[-1]:
        bot.send_message(message.chat.id, 'Вернулись в главное меню. Что будем делать?', reply_markup = main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return


    selected_queue = [queue for queue in possible_queues if queue['name'] == message.text] [0]
    tickets = Handlers.get_queue_tickets(selected_queue, session_id)
    if tickets is None or len(tickets) == 0:
        marks = create_markup(['✒️ Редактировать очередь', '➕ Создать задание','📅 В главное меню'])
        bot.send_message(message.chat.id, 'Заданий ещё нет. Создадим новое  ?', reply_markup = marks)
        bot.register_next_step_handler(message, create_ticket, selected_queue)
        return   
    else:
        ticket_names = ['✒️ Редактировать очередь']+[ticket['title'] for ticket in tickets] + ['➕ Создать задание','📅 В главное меню']
        tickets_markup = create_markup(ticket_names)
        bot.send_message(message.chat.id, f'Список заданий очереди {selected_queue['name']}:', reply_markup = tickets_markup)
        bot.register_next_step_handler(message, ticket_info, selected_queue)
    
def edit_queue(message:Message, current_queue):
    marks = create_markup(['➕ Добавить пользователя','❌ Исключить пользователя', '📅 Главное меню'])
    bot.send_message(message.chat.id, 'Что будем делать?', reply_markup = marks)
    bot.register_next_step_handler(message, edit_queue2, current_queue)
    
def edit_queue2(message:Message, current_queue):
    session_id = get_user_info(message.from_user.username)['session_id']

    posible_answers = ['➕ Добавить пользователя','❌ Исключить пользователя', '📅 Главное меню']
    if message.text is None or message.text not in posible_answers:
        marks = create_markup(posible_answers)
        bot.send_message(message.chat.id, 'Я вас не понял', reply_markup = marks)
        bot.register_next_step_handler(message, edit_queue2, current_queue)
        return
    
    if message.text == posible_answers[-1]:
        marks = create_markup(posible_answers)
        bot.send_message(message.chat.id, 'Возвращаемся в главное меню', reply_markup = main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    
    if message.text == posible_answers[0]:
        bot.send_message(message.chat.id, 'Отправьте мне логин пользователя, которого хотите добавить в очередь в формате @username. Или напишите "Отмена"')
        bot.register_next_step_handler(message, add_user_to_queue, current_queue)
        return
    elif message.text == posible_answers[1]:
        users = Handlers.get_queue_users(session_id, current_queue['id'])
        user_names = [user['login'] for user in users]
        user_names.append('❌ Отмена')
        user_marks = create_markup(user_names)
        bot.send_message(message.chat.id, 'Кого будем убирать?', reply_markup=user_marks)#, reply_markup=
        bot.register_next_step_handler(message, remove_user_from_queue, current_queue)
        return
    
def make_uniq(l:list):
    unique_list = []
    seen = set()
    for item in l:
        if item not in seen:
            unique_list.append(item)
            seen.add(item)
    return unique_list

def add_user_to_queue(message:Message, current_queue):
    session_id = get_user_info(message.from_user.username)['session_id']
    if message.text == 'Отмена':
        bot.send_message(message.chat.id, 'Возвращаемся в главное меню', reply_markup = main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return

    if message.text == None or not message.text.startswith('@'):
        bot.send_message(message.chat.id, 'Не валидный логин. Попробуйте ещё раз, в формате (@username)')
        bot.register_next_step_handler(message, add_user_to_queue, current_queue)
        return
    user = Handlers.get_user_by_login(session_id, message.text)
    if user == '_':
        bot.send_message(message.chat.id, 'Не нашли такого пользователя. Попробуйте ещё раз, в формате (@username)')
        bot.register_next_step_handler(message, add_user_to_queue, current_queue)
        return


    if Handlers.add_user_to_queue(session_id, current_queue['id'], user['id']):
        bot.send_message(message.chat.id, f'Пользователь успешно добавлен в очередь')
        bot.send_message(message.chat.id, 'Возращаемся в главное меню', reply_markup=main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    
    else:
        bot.send_message(message.chat.id, f'Не удалось ➕ Добавить пользователя')
        bot.send_message(message.chat.id, 'Возращаемся в главное меню', reply_markup=main_menu_markup)
        bot.register_next_step_handler(message, main_menu)


def remove_user_from_queue(message:Message, current_queue):
    session_id = get_user_info(message.from_user.username)['session_id']
    if message.text == '❌ Отмена':
        bot.send_message(message.chat.id, 'Возвращаемся в главное меню', reply_markup = main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    
    if message.text == None or not message.text.startswith('@'):
        bot.send_message(message.chat.id, 'Не валидный логин. Попробуйте ещё раз, в формате (@username)')
        bot.register_next_step_handler(message, add_user_to_queue, current_queue)
        return
    user = Handlers.get_user_by_login(session_id, message.text)
    if user == '_':
        bot.send_message(message.chat.id, 'Не нашли такого пользователя. Попробуйте ещё раз, в формате (@username)')
        bot.register_next_step_handler(message, add_user_to_queue, current_queue)
        return
    
    if Handlers.remove_user_from_queue(session_id, current_queue['id'], user['id']):
        bot.send_message(message.chat.id, f'Пользователь удалён из очереди')
        bot.send_message(message.chat.id, 'Возращаемся в главное меню', reply_markup=main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    
    else:
        bot.send_message(message.chat.id, f'Не удалось удалить пользователя')
        bot.send_message(message.chat.id, 'Возращаемся в главное меню', reply_markup=main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
    


def create_ticket(message:Message, current_queue):
    if message.text == '✒️ Редактировать очередь':
        edit_queue(message, current_queue)
        return
    marks_names = ['➕ Создать задание','📅 Главное меню']
    marks = create_markup(marks_names)

    if message.text is None or message.text not in marks_names :
        bot.send_message(message.chat.id, 'Я вас не понял, давайте попробуем ещё раз', reply_markup = marks)
        bot.register_next_step_handler(message, create_ticket, current_queue)
        return
    
    if message.text == marks_names[-1]:
        bot.send_message(message.chat.id, 'Вернулись в главное меню. Что будем делать?', reply_markup = main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    
    if message.text == marks_names[0]:
        bot.send_message(message.chat.id, 'Укажите название нового задания')
        bot.register_next_step_handler(message, task_name, current_queue)
        return
    
def task_name(message:Message, cureent_queue):
    if message.text is None:
        bot.send_message(message.chat.id, 'Я не нашёл текст, попробуем ещё раз')
        bot.register_next_step_handler(message, task_name, cureent_queue)
        return
    
    bot.send_message(message.chat.id, 'Получил название, теперь добавим описание задачи')
    bot.register_next_step_handler(message, task_description, cureent_queue, message.text)

def task_description(message:Message, cureent_queue, task_name):
    session_id = get_user_info(message.from_user.username)['session_id']

    if message.text is None:
        bot.send_message(message.chat.id, 'Я не нашёл текст, попробуем ещё раз')
        bot.register_next_step_handler(message, task_description, cureent_queue, task_name)
        return
    
    bot.send_message(message.chat.id, 'Получил описание')
    description = message.text
    hardcode_status = 1
    if Handlers.create_ticket(session_id, cureent_queue['id'], task_name, description, hardcode_status):
        bot.send_message(message.chat.id, 'Задача создана, возвращаемся в главное меню, что будем делать?', reply_markup = main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    else:
        bot.send_message(message.chat.id, 'Не удалось создать задачу. Возвращаемся в главное меню', reply_markup = main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return

def ticket_info(message:Message, current_queue):
    if message.text == '✒️ Редактировать очередь':
        edit_queue(message, current_queue)
        return
    
    session_id = get_user_info(message.from_user.username)['session_id']
    if message.text == '📅 В главное меню':
        bot.send_message(message.chat.id, 'Вернулись в главное меню. Что будем делать?', reply_markup = main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    if message.text == '➕ Создать задание':
        create_ticket(message, current_queue)
        return

    tickets = Handlers.get_queue_tickets(current_queue, session_id)
    if tickets is None or len(tickets) == 0:
        bot.send_message(message.chat.id, 'Что-то пошло не так. Возвращаемся в галвное меню', reply_markup= main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    ticket_names = [queue['title'] for queue in tickets]
    if message.text is None or message.text not in ticket_names:
        queues_markup = create_markup(ticket_names)
        queues_markup.add('📅 В главное меню')
        bot.send_message(message.chat.id, 'Я вас не понял', reply_markup= queues_markup)
        bot.register_next_step_handler(message, ticket_info, current_queue)
        return
    
    curent_ticket = [ticket for ticket in tickets if ticket['title'] == message.text][0]
    status_name =''
    status_id = int(curent_ticket['status_id'])
    if(status_id == 1 ):
        status_name = 'Открыт'
    elif (status_id == 2):
        status_name = 'В работе'
    else:
        status_name = 'Завершено'
    
        
    if 'performer_id' in curent_ticket:
        users = Handlers.get_all_users(session_id)
        performer_login = [user['login'] for user in users if int(user['id']) == int(curent_ticket['performer_id'])][0]  
    else:
        performer_login = 'Нет'
    answer = f'''
*Название:* {clear_string(curent_ticket['title'])}
*Описание:* {clear_string(curent_ticket['description'])}
*Исполнитель:* {clear_string(performer_login)}
*Статус:* {clear_string(curent_ticket['status_id'])}
*Приоритет:* {clear_string(curent_ticket['priority'])}
*Статус:* {clear_string(status_name)}
*Дата создания:* {clear_string(curent_ticket['created_date'][:10])}
*Автор:* {clear_string(curent_ticket['author_login'])}
    '''

    bot.send_message(message.chat.id, answer, parse_mode='Markdown')
    marks = create_markup(['🛠️ Сменить статус','🎯 Назначить(сменить) исполнителя', '📅 В главное меню'])
    bot.send_message(message.chat.id, 'Что будем делать?', reply_markup= marks)
    bot.register_next_step_handler(message, edit_ticket, curent_ticket)

def clear_string(string:str):
    return str(string).replace('[','').replace(']','').replace('_','').replace('*','')

def edit_ticket(message:Message, ticket):
    posible_answers = ['🛠️ Сменить статус','🎯 Назначить(сменить) исполнителя', '📅 В главное меню']
    if message.text == '📅 В главное меню':
        bot.send_message(message.chat.id, 'Вернулись в главное меню. Что будем делать?', reply_markup = main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    if message.text is None or message.text not in posible_answers :
        marks = create_markup(posible_answers)
        bot.send_message(message.chat.id, 'Я вас не понял')
        bot.send_message(message.chat.id, 'Что будем делать?', reply_markup= marks)
        bot.register_next_step_handler(message, edit_ticket, ticket)
        return
    elif message.text == posible_answers[0]:
        marks = create_markup(['Открыт','В работе','Завершено'])
        bot.send_message(message.chat.id, 'На какой статус будем менять?', reply_markup= marks)
        bot.register_next_step_handler(message, change_status, ticket)
        return 
    
    elif message.text == posible_answers[1]:
        marks = create_markup(['Открыт','В работе','Завершено'])
        bot.send_message(message.chat.id, 'Отправьте логин пользователя, чтобы назначить ему задачу.')
        bot.send_message(message.chat.id, 'Чтобы назначить задачу - пользователь должен быть зарегестрирован в системе')
        bot.register_next_step_handler(message, change_performer, ticket)
        return
    

def change_status(message:Message, ticket):
    marks_names = ['Открыт','В работе','Завершено']

    if message.text is None or message.text not in marks_names:
        marks = ['Открыт','В работе','Завершено']
        bot.send_message(message.chat.id, 'Я вас не понял')
        bot.send_message(message.chat.id, 'На какой статус будем менять?', reply_markup= marks)
        bot.register_next_step_handler(message, change_status)
        return
    session_id = get_user_info(message.from_user.username)['session_id']


    if Handlers.update_ticket(session_id, ticket, marks_names.index(message.text), None):
        bot.send_message(message.chat.id, 'Статус успешно изменен, возвращаемся в главное меню', reply_markup= main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    else:
        bot.send_message(message.chat.id, 'Не удалось сменить статус. Возвращаемся в главное меню', reply_markup= main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return

def change_performer(message:Message, ticket):
    if message.text == None or not message.text.startswith('@'):
        bot.send_message(message.chat.id, 'Не валидный логин. Попробуйте ещё раз, в формате (@username)')
        bot.register_next_step_handler(message, change_performer, ticket)
        return
    
    session_id = get_user_info(message.from_user.username)['session_id']
    user = Handlers.get_user_by_login(session_id, message.text)
    if user == '_':
        bot.send_message(message.chat.id, 'Не нашли такого пользователя. Попробуйте ещё раз, в формате (@username)')
        bot.register_next_step_handler(message, change_performer, ticket)
        return
    
    if Handlers.update_ticket(session_id, ticket, None, user['id']):
        bot.send_message(message.chat.id, f'Задача успешно назначена пользователю {user['login']} ')
        bot.send_message(message.chat.id, 'Возращаемся в главное меню', reply_markup=main_menu_markup)
        bot.register_next_step_handler(message, main_menu)
        return
    
    else:
        bot.send_message(message.chat.id, f'Не удалось сменить пользователя')
        bot.send_message(message.chat.id, 'Возращаемся в главное меню', reply_markup=main_menu_markup)
        bot.register_next_step_handler(message, main_menu)


class Handlers:
    @staticmethod
    def login(user_name, password):
       
        Handlers.register(user_name, password) 
        url = get_path_to_handler('login')
        multipart_data = MultipartEncoder(
        fields={
            "login": user_name,
            "password": password
        })
        r = requests.post(url=url, data=multipart_data, headers={"Content-Type": multipart_data.content_type}, timeout=5)
        if(r.status_code != 200):
            print('Не удалось залогинится', r)
            return None
        response = r.json()

        user_info = {'login':user_name, 'password':password, 'session_id':response['id']}
        return user_info

    @staticmethod
    def register(user_name, password):
        url = get_path_to_handler('register')
        multipart_data = MultipartEncoder(
        fields={
            "login": user_name,
            "password": password
        })    

        r = requests.post(url=url, data=multipart_data, headers={"Content-Type": multipart_data.content_type}, timeout=5)
        if(r.status_code != 200):
            print('Не удалось зарегаться', r)
            return None
        response = r.json()
        return response['id']
    
    @staticmethod
    def unregister():
        pass
    @staticmethod
    def get_all_queues(session_id):
        url = get_path_to_handler('queues')    
        headers = {'X-Ya-User-Ticket': session_id}

        r = requests.get(url=url,headers=headers, timeout=5)
        if(r.status_code != 200):
            print('Не удалось получить очереди', r)
            return None
        response = r.json()
        return response['items']
    
    @staticmethod
    def create_queue(queue_name, session_id):
        url = get_path_to_handler('queues')
        data = {
            'name':queue_name
        }    
        headers = {'X-Ya-User-Ticket': session_id}
        r = requests.post(url=url, json=data, headers=headers, timeout=5)
        if(r.status_code != 200):
            print('Не удалось создать очередь. Такое название уже занято', r)
            return None
        response = r.json()
        return response
    
    @staticmethod
    def get_queue_tickets(queue, session_id):
        url = get_path_to_handler(f'queues/{queue['id']}/tickets')  
        
        headers = {'X-Ya-User-Ticket': session_id}
        r = requests.get(url=url, headers=headers, timeout=5)
        if(r.status_code != 200):
            print('Не удалось получить задание', r)
            return None
        response = r.json()
        return response['items']


    @staticmethod
    def get_user_by_login(session_id, username:str):
        if username.startswith('@'):
            username = username.replace('@','')

        print(username)
        url = get_path_to_handler(f'users/{username}')
        headers = {'X-Ya-User-Ticket': session_id}
        r = requests.get(url=url, headers=headers, timeout=5)
        if(r.status_code != 200):
            print('Не удалось получить пользователя', r)
            return '_'
        response = r.json()
        return response

    @staticmethod
    def get_all_users(session_id):
        
        url = get_path_to_handler(f'users')
        headers = {'X-Ya-User-Ticket': session_id}
        r = requests.get(url=url, headers=headers, timeout=5)
        if(r.status_code != 200):
            print('Не удалось получить пользователей', r)
            return '_'
        response = r.json()
        return response['items']
    
    @staticmethod
    def update_queue():
        pass
    @staticmethod
    def delete_queue():
        pass
    @staticmethod
    def get_queue_users(session_id, queue_id):
        url = get_path_to_handler(f'queues/{queue_id}/users')   
        headers = {'X-Ya-User-Ticket': session_id}
        r = requests.get(url=url, headers=headers, timeout=5)
        if(r.status_code != 200):
            print('Не удалось ➕ Добавить пользователя в очередь', r)
            return None
        response = r.json()
        return response['items']

    
    @staticmethod
    def add_user_to_queue(session_id, queue_id, user_id):
        url = get_path_to_handler(f'queues/{queue_id}/users/{user_id}')   
        headers = {'X-Ya-User-Ticket': session_id}
        r = requests.post(url=url, headers=headers, timeout=5)
        if(r.status_code != 200):
            print('Не удалось ➕ Добавить пользователя в очередь', r)
            return None
        response = r.json()
        return response
        

    @staticmethod
    def remove_user_from_queue(session_id, queue_id, user_id):
        url = get_path_to_handler(f'queues/{queue_id}/users/{user_id}')   
        headers = {'X-Ya-User-Ticket': session_id}
        r = requests.delete(url=url, headers=headers, timeout=5)
        if(r.status_code != 200):
            print('Не удалось удалить пользователя из очереди', r)
            return None
        response = r.json()
        return response
    
    @staticmethod
    def create_ticket(session_id, queue_id, name, description, status):
        url = get_path_to_handler('tickets')
        data = {
            'title':name,
            'description': description,
            'status_id': status,
            'queue_id': queue_id,
        }    
        headers = {'X-Ya-User-Ticket': session_id}
        r = requests.post(url=url, json=data, headers=headers, timeout=5)
        if(r.status_code != 200):
            print('Не удалось создать новый ticket', r)
            return None
        response = r.json()
        return response

    @staticmethod
    def get_ticket():
        pass


    @staticmethod
    def update_ticket(session_id, ticket, status_id, performer_id):
        url = get_path_to_handler(f'tickets/{ticket['id']}')
        if status_id is not None:
            ticket['status_id'] =  status_id + 1
        if performer_id is not None:
            ticket['performer_id'] =  performer_id
            
        headers = {'X-Ya-User-Ticket': session_id}
        r = requests.post(url=url, json=ticket, headers=headers, timeout=5)
        if(r.status_code != 200):
            print('Не удалось изменить ticket', r)
            return None
        response = r.json()
        return response
    
    @staticmethod
    def delete_ticket():
        pass
    @staticmethod
    def get_all_comments():
        pass
    @staticmethod
    def create_comment():
        pass
    @staticmethod
    def get_comment():
        pass
    @staticmethod
    def update_comment():
        pass
    @staticmethod
    def delete_comment():
        pass

print('Запустили бота')
start_bot()
