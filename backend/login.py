from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from flask_cors import CORS
import time

app = Flask(__name__)

# 跨域配置：允许前端 localhost:5173 访问，支持带 cookie
CORS(app, resources={r"*": {"origins": "http://localhost:5173"}},
     supports_credentials=True,
     allow_headers=["Content-Type", "Authorization"])

# 数据库配置（保持不变）
HOSTNAME = "127.0.0.1"
PORT = 3306
USERNAME = "root"
PASSWORD = "923918399qwe"
DATABASE = "bankdb"

app.config['SQLALCHEMY_DATABASE_URI'] = f"mysql+pymysql://{USERNAME}:{PASSWORD}@{HOSTNAME}:{PORT}/{DATABASE}?charset=utf8"
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db = SQLAlchemy(app)

# 数据库模型：适配银行卡号字段
class LoginList(db.Model):
    __tablename__ = "loginlist"  # 明确表名（可选，默认用类名小写）
    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    username = db.Column(db.String(50), unique=True, nullable=False)  # 用户名唯一，非空
    password = db.Column(db.String(100), nullable=False)  # 密码非空
    card = db.Column(db.String(20), unique=True, nullable=False)  # 银行卡号（唯一，非空，适配前端）
    isvalid = db.Column(db.String(20), default="使用中")  # 账户状态：使用中/封禁中
    lasttime = db.Column(db.String(50))  # 最后登录时间
    balance = db.Column(db.Float, default=0.0)
    
    def __repr__(self):
        return f"<LoginList {self.username}>"
# 新增申请记录表（存储用户申请及审核状态）
class ApplyRecord(db.Model):
    __tablename__ = "apply_records"
    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    username = db.Column(db.String(50), nullable=False)  # 申请人用户名
    apply_type = db.Column(db.String(20), nullable=False)  # 申请类型：挂失/冻结/销户
    apply_status = db.Column(db.String(20), default="待审核")  # 审核状态：待审核/通过/驳回
    apply_time = db.Column(db.String(50), default=lambda: time.strftime('%Y-%m-%d %H:%M:%S'))  # 申请时间
    audit_time = db.Column(db.String(50), nullable=True)  # 审核时间
    auditor = db.Column(db.String(50), nullable=True)  # 审核人（管理员用户名）


# 自动创建表（如果表不存在）
with app.app_context():
    db.create_all()

# ---------------------- 核心接口：登录 ----------------------
@app.route('/login', methods=['POST'])
def login():
    try:
        # 1. 获取前端传递的 JSON 数据
        data = request.get_json()
        if not data:
            return jsonify({'message': '请传递有效数据！'}), 400
        
        # 2. 提取登录核心字段（与前端表单绑定字段一致）
        username = data.get('username').strip()  # 去除空格
        password = data.get('password').strip()
        login_time = data.get('lasttime')  # 前端传递的登录时间（可选）
        
        # 3. 字段校验（非空）
        if not username or not password:
            return jsonify({'message': '用户名或密码不能为空！'}), 400
        
        # 4. 数据库查询：根据用户名查找用户
        user = LoginList.query.filter_by(username=username).first()
        
        # 5. 登录逻辑判断
        if not user:
            # 用户名不存在
            return jsonify({'message': '用户不存在，请先注册！'}), 404
        if user.isvalid != '使用中':
            # 账户被封禁
            return jsonify({'message': '当前账户已被封禁，无法登录！'}), 403
        if user.password != password:
            # 密码错误
            return jsonify({'message': '密码错误，请重新输入！'}), 401
        
        # 6. 更新最后登录时间（优先用前端传递的时间，否则用服务器时间）
        user.lasttime = login_time or time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        db.session.commit()  # 提交数据库修改
        
        # 7. 登录成功：返回成功信息（前端接收后跳转 /home）
        return jsonify({
            'message': '登录成功！',
            'username': user.username,  # 可选：返回用户名给前端存储
            'card': user.card  # 可选：返回银行卡号给前端
        }), 200
    
    except Exception as e:
        # 异常处理（如数据库错误）
        db.session.rollback()  # 回滚事务
        return jsonify({'message': f'登录失败：{str(e)}'}), 500

# ---------------------- 辅助接口：注册（适配银行卡号） ----------------------
@app.route('/sign', methods=['POST'])
def sign_up():
    try:
        data = request.get_json()
        if not data:
            return jsonify({'message': '请传递有效数据！'}), 400
        
        # 提取注册字段（前端 cardNumber 映射到后端 card）
        username = data.get('username').strip()
        password = data.get('password').strip()
        card = data.get('card').strip()  # 银行卡号（前端传递的 cardNumber 字段）
        
        # 字段校验
        if not username or not password or not card:
            return jsonify({'message': '用户名、密码、银行卡号不能为空！'}), 400
        # 银行卡号格式校验（简单校验：16-19位数字）
        if not card.isdigit() or len(card) not in (10,11,12,13,14,15,16):
            return jsonify({'message': '银行卡号格式错误（需为10-16位数字）！'}), 400
        
        # 校验用户名/银行卡号是否已存在
        if LoginList.query.filter_by(username=username).first():
            return jsonify({'message': '用户名已被占用，请更换！'}), 400
        if LoginList.query.filter_by(card=card).first():
            return jsonify({'message': '该银行卡号已注册，请更换！'}), 400
        
        # 创建新用户
        new_user = LoginList(
            username=username,
            password=password,
            card=card,
            lasttime=time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        )
        db.session.add(new_user)
        db.session.commit()
        
        return jsonify({'message': '注册成功，请登录！'}), 201
    
    except Exception as e:
        db.session.rollback()
        return jsonify({'message': f'注册失败：{str(e)}'}), 500

# ---------------------- 辅助接口：修改账户状态（封禁/解封） ----------------------
@app.route('/changestate', methods=['POST'])
def change_state():
    try:
        data = request.get_json()
        username = data.get('username')
        state = data.get('state')
        
        if not username or not state:
            return jsonify({'message': '用户名和状态不能为空！'}), 400
        
        user = LoginList.query.filter_by(username=username).first()
        if not user:
            return jsonify({'message': '用户不存在！'}), 404
        
        # 更新状态（仅支持 "使用中" 和 "封禁中"）
        if state in ('使用中', '封禁中'):
            user.isvalid = state
            db.session.commit()
            return jsonify({'message': f'账户已{state}！'}), 200
        else:
            return jsonify({'message': '状态只能是「使用中」或「封禁中」！'}), 400
    
    except Exception as e:
        db.session.rollback()
        return jsonify({'message': f'状态修改失败：{str(e)}'}), 500

# ---------------------- 辅助接口：获取所有用户列表 ----------------------
@app.route('/getuserlist', methods=['GET'])
def get_user_list():
    try:
        users = LoginList.query.all()
        # 格式化返回数据
        user_list = [
            {
                'id': user.id,
                'username': user.username,
                'card': user.card,  # 返回银行卡号
                'isvalid': user.isvalid,
                'lasttime': user.lasttime
            } for user in users
        ]
        return jsonify(user_list), 200
    except Exception as e:
        return jsonify({'message': f'获取用户列表失败：{str(e)}'}), 500

# ---------------------- 辅助接口：修改密码 ----------------------
@app.route('/change_password', methods=['POST'])
def change_password():
    try:
        data = request.get_json()
        if not data:
            return jsonify({'message': '请求数据无效！'}), 400

        username = data.get('username')
        old_password = data.get('old_password')
        new_password = data.get('new_password')

        # 参数校验
        if not username or not old_password or not new_password:
            return jsonify({'message': '用户名、旧密码、新密码不能为空！'}), 400
        if old_password == new_password:
            return jsonify({'message': '新密码不能与旧密码相同！'}), 400

        # 查询用户
        user = LoginList.query.filter_by(username=username).first()
        if not user:
            return jsonify({'message': '用户不存在！'}), 404

        # 验证旧密码
        if user.password != old_password:
            return jsonify({'message': '旧密码错误！'}), 401

        # 更新新密码
        user.password = new_password
        db.session.commit()

        return jsonify({'message': '密码修改成功！'}), 200

    except Exception as e:
        db.session.rollback()
        return jsonify({'message': f'密码修改失败：{str(e)}'}), 500

@app.route('/get_balance', methods=['GET'])
def get_balance():
    username = request.args.get('username')
    if not username:
        return jsonify({'message': '缺少用户名'}), 400

    user = LoginList.query.filter_by(username=username).first()
    if not user:
        return jsonify({'message': '用户不存在'}), 404

    return jsonify({'balance': user.balance}), 200


@app.route('/update_balance', methods=['POST'])
def update_balance():
    data = request.get_json()
    username = data.get('username')
    change = data.get('change')  # 可以是正数（存款）或负数（取款）

    if username is None or change is None:
        return jsonify({'message': '缺少参数'}), 400

    user = LoginList.query.filter_by(username=username).first()
    if not user:
        return jsonify({'message': '用户不存在'}), 404

    # 检查余额是否足够
    new_balance = user.balance + float(change)
    if new_balance < 0:
        return jsonify({'message': '余额不足'}), 400

    # 更新数据库
    user.balance = new_balance
    db.session.commit()

    return jsonify({'message': '操作成功', 'balance': user.balance}), 200

# ---------------------- 辅助接口：删除用户 ----------------------
@app.route('/deleteuser', methods=['POST'])
def delete_user():
    try:
        data = request.get_json()
        user_id = data.get('id')
        
        if not user_id:
            return jsonify({'message': '用户ID不能为空！'}), 400
        
        user = LoginList.query.get(user_id)
        if not user:
            return jsonify({'message': '用户不存在！'}), 404
        
        db.session.delete(user)
        db.session.commit()
        return jsonify({'message': '用户删除成功！'}), 200
    
    except Exception as e:
        db.session.rollback()
        return jsonify({'message': f'删除用户失败：{str(e)}'}), 500

# ---------------------- 账户挂失申请接口（仅提交申请，不直接修改状态） ----------------------
@app.route('/apply_loss', methods=['POST'])
def apply_loss():
    try:
        data = request.get_json()
        username = data.get('username')
        if not username:
            return jsonify({'message': '缺少用户名'}), 400
        
        user = LoginList.query.filter_by(username=username).first()
        if not user:
            return jsonify({'message': '用户不存在'}), 404
        # 禁止重复申请或异常状态申请
        if user.isvalid in ('已销户', '处理中'):
            return jsonify({'message': f'{user.isvalid}账户无法提交挂失申请'}), 400
        # 检查是否有未审核的同类型申请
        existing_apply = ApplyRecord.query.filter_by(
            username=username, apply_type='挂失', apply_status='待审核'
        ).first()
        if existing_apply:
            return jsonify({'message': '已有挂失申请待审核，请勿重复提交'}), 400
        
        # 新增申请记录
        new_apply = ApplyRecord(username=username, apply_type='挂失')
        db.session.add(new_apply)
        db.session.commit()
        return jsonify({'message': '挂失申请已提交，请等待管理员审核'}), 200
    except Exception as e:
        db.session.rollback()
        return jsonify({'message': f'挂失申请失败：{str(e)}'}), 500

# ---------------------- 账户冻结申请接口 ----------------------
@app.route('/apply_freeze', methods=['POST'])
def apply_freeze():
    try:
        data = request.get_json()
        username = data.get('username')
        if not username:
            return jsonify({'message': '缺少用户名'}), 400
        
        user = LoginList.query.filter_by(username=username).first()
        if not user:
            return jsonify({'message': '用户不存在'}), 404
        if user.isvalid in ('已销户', '处理中'):
            return jsonify({'message': f'{user.isvalid}账户无法提交冻结申请'}), 400
        existing_apply = ApplyRecord.query.filter_by(
            username=username, apply_type='冻结', apply_status='待审核'
        ).first()
        if existing_apply:
            return jsonify({'message': '已有冻结申请待审核，请勿重复提交'}), 400
        
        new_apply = ApplyRecord(username=username, apply_type='冻结')
        db.session.add(new_apply)
        db.session.commit()
        return jsonify({'message': '冻结申请已提交，请等待管理员审核'}), 200
    except Exception as e:
        db.session.rollback()
        return jsonify({'message': f'冻结申请失败：{str(e)}'}), 500

# ---------------------- 账户销户申请接口（新增余额校验） ----------------------
@app.route('/apply_close', methods=['POST'])
def apply_close():
    try:
        data = request.get_json()
        username = data.get('username')
        if not username:
            return jsonify({'message': '缺少用户名'}), 400
        
        user = LoginList.query.filter_by(username=username).first()
        if not user:
            return jsonify({'message': '用户不存在'}), 404
        if user.isvalid in ('已销户', '处理中'):
            return jsonify({'message': f'{user.isvalid}账户无法提交销户申请'}), 400
        # 销户必须余额为0
        if user.balance > 0:
            return jsonify({'message': '账户余额不为0，请先提现再申请销户'}), 400
        existing_apply = ApplyRecord.query.filter_by(
            username=username, apply_type='销户', apply_status='待审核'
        ).first()
        if existing_apply:
            return jsonify({'message': '已有销户申请待审核，请勿重复提交'}), 400
        
        new_apply = ApplyRecord(username=username, apply_type='销户')
        db.session.add(new_apply)
        db.session.commit()
        return jsonify({'message': '销户申请已提交，请等待管理员审核'}), 200
    except Exception as e:
        db.session.rollback()
        return jsonify({'message': f'销户申请失败：{str(e)}'}), 500

# ---------------------- 获取账户当前状态接口 ----------------------
@app.route('/get_account_status', methods=['GET'])
def get_account_status():
    try:
        username = request.args.get('username')
        if not username:
            return jsonify({'message': '缺少用户名'}), 400
        
        user = LoginList.query.filter_by(username=username).first()
        if not user:
            return jsonify({'message': '用户不存在'}), 404
        
        return jsonify({
            'status': user.isvalid,
            'username': user.username,
            'card': user.card
        }), 200
    except Exception as e:
        return jsonify({'message': f'获取状态失败：{str(e)}'}), 500


# ---------------------- 管理员获取待审核申请列表 ----------------------
@app.route('/get_pending_applies', methods=['GET'])
def get_pending_applies():
    try:
        # 关联查询申请记录和用户信息
        pending_applies = db.session.query(ApplyRecord, LoginList).join(
            LoginList, ApplyRecord.username == LoginList.username
        ).filter(ApplyRecord.apply_status == '待审核').all()
        
        result = []
        for apply, user in pending_applies:
            result.append({
                'apply_id': apply.id,
                'username': apply.username,
                'card': user.card,
                'apply_type': apply.apply_type,
                'apply_time': apply.apply_time,
                'current_status': user.isvalid,
                'balance': user.balance
            })
        return jsonify(result), 200
    except Exception as e:
        return jsonify({'message': f'获取待审核申请失败：{str(e)}'}), 500

# ---------------------- 管理员审核申请接口 ----------------------
@app.route('/audit_apply', methods=['POST'])
def audit_apply():
    try:
        data = request.get_json()
        apply_id = data.get('apply_id')  # 申请ID
        audit_result = data.get('audit_result')  # 通过/驳回
        auditor = data.get('auditor')  # 管理员用户名
        
        if not apply_id or not audit_result or not auditor:
            return jsonify({'message': '申请ID、审核结果、审核人均不能为空'}), 400
        if audit_result not in ('通过', '驳回'):
            return jsonify({'message': '审核结果只能是「通过」或「驳回」'}), 400
        
        # 查询申请记录和用户
        apply_record = ApplyRecord.query.get(apply_id)
        if not apply_record:
            return jsonify({'message': '申请记录不存在'}), 404
        if apply_record.apply_status != '待审核':
            return jsonify({'message': '该申请已审核，无需重复操作'}), 400
        
        user = LoginList.query.filter_by(username=apply_record.username).first()
        if not user:
            return jsonify({'message': '申请人不存在'}), 404
        
        # 更新申请记录状态
        apply_record.apply_status = audit_result
        apply_record.audit_time = time.strftime('%Y-%m-%d %H:%M:%S')
        apply_record.auditor = auditor
        
        # 审核通过：修改用户状态；驳回：恢复为原状态（使用中）
        if audit_result == '通过':
            if apply_record.apply_type == '挂失':
                user.isvalid = '挂失'
            elif apply_record.apply_type == '冻结':
                user.isvalid = '冻结'
            elif apply_record.apply_type == '销户':
                user.isvalid = '已销户'
        else:
            user.isvalid = '使用中'  # 驳回后恢复为使用中
        
        db.session.commit()
        return jsonify({'message': f'申请审核{audit_result}成功'}), 200
    except Exception as e:
        db.session.rollback()
        return jsonify({'message': f'审核失败：{str(e)}'}), 500

# ---------------------- 管理员获取所有用户状态（含处理中） ----------------------
@app.route('/get_all_users', methods=['GET'])
def get_all_users():
    try:
        users = LoginList.query.all()
        user_list = [
            {
                'id': user.id,
                'username': user.username,
                'card': user.card,
                'isvalid': user.isvalid,
                'lasttime': user.lasttime,
                'balance': user.balance
            } for user in users
        ]
        return jsonify(user_list), 200
    except Exception as e:
        return jsonify({'message': f'获取用户列表失败：{str(e)}'}), 500
        
# 运行服务器
if __name__ == '__main__':
    app.run(port=5003, debug=True)  # 端口 5003，与前端请求地址一致