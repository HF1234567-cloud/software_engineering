from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from flask_cors import CORS
import time

app = Flask(__name__)

# ✅ 修改：所有接口都允许跨域，支持带 cookie
CORS(app, resources={r"*": {"origins": "http://localhost:5173"}},
     supports_credentials=True,
     allow_headers=["Content-Type", "Authorization"])

# 数据库配置
HOSTNAME = "127.0.0.1"
PORT = 3306
USERNAME = "root"
PASSWORD = "923918399qwe"
DATABASE = "backplotform"

app.config['SQLALCHEMY_DATABASE_URI'] = f"mysql+pymysql://{USERNAME}:{PASSWORD}@{HOSTNAME}:{PORT}/{DATABASE}?charset=utf8"
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db = SQLAlchemy(app)

# 数据库模型
class loginlist(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.Text)
    password = db.Column(db.Text)
    phone = db.Column(db.Text)
    isvalid = db.Column(db.Text, default="使用中")
    lasttime = db.Column(db.Text)
    
    def __repr__(self):
        return f"<loginlist {self.id}>"

# 自动创建表
with app.app_context():
    db.create_all()

# 登录接口
@app.route('/login', methods=['POST'])
def login():
    data = request.get_json()
    username = data.get('username')
    password = data.get('password')
    lasttime = data.get('lasttime')

    if not username or not password:
        return jsonify({'message': '用户名或密码不能为空！'}), 400

    user = loginlist.query.filter_by(username=username).first()
    if not user:
        return jsonify({'message': '用户不存在'}), 404

    if user.isvalid != '使用中':
        return jsonify({'message': '当前账户被禁用！'}), 403

    if user.password != password:
        return jsonify({'message': '密码错误！'}), 401

    user.lasttime = lasttime or time.strftime('%Y-%m-%d %H:%M', time.localtime())
    db.session.commit()
    return jsonify({'message': '登录成功！'}), 200

# 注册接口
@app.route('/sign', methods=['POST'])
def sign():
    data = request.get_json()
    username = data.get('username')
    
    user = loginlist.query.filter_by(username=username).first()
    if user:
        return jsonify({'message': '用户已存在'}), 400

    new_user = loginlist(
        username=data['username'],
        password=data['password'],
        phone=data['phone'],
        isvalid="使用中",
        lasttime=time.strftime('%Y-%m-%d %H:%M', time.localtime())
    )
    db.session.add(new_user)
    db.session.commit()
    return jsonify({'message': '注册成功'}), 200

# 修改用户状态
@app.route('/changestate', methods=['POST'])
def changestate():
    data = request.get_json()
    username = data.get('username')
    state = data.get('state')

    user = loginlist.query.filter_by(username=username).first()
    if user:
        user.isvalid = '使用中' if state == '使用中' else '封禁中'
        db.session.commit()
        return jsonify({'message': '修改成功'}), 200
    else:
        return jsonify({'message': '用户不存在'}), 400

# 获取用户列表
@app.route('/getuserlist', methods=['GET'])
def getuserlist():
    users = loginlist.query.all()
    user_dict = [
        {'id': user.id, 'username': user.username, 'password': user.password, 
         'phone': user.phone, 'isvalid': user.isvalid, 'lasttime': user.lasttime} 
        for user in users
    ]
    return jsonify(user_dict)

# 删除用户
@app.route('/deleteuser', methods=['POST'])
def deleteuser():
    data = request.get_json()
    id = data.get('id')
    record = loginlist.query.get(id)
    if not record:
        return jsonify({'message': '用户不存在'}), 400
    db.session.delete(record)
    db.session.commit()
    return jsonify({'message': '删除成功'}), 200

if __name__ == '__main__':
    app.run(port=5003, debug=True)
