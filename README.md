<h3>项目介绍</h3>
项目以<b>ActionRPG</b>为模板，接入<b>TapTap开发者服务</b><br>
项目通过UE_SDK来接入服务<br>
项目目前(v1.0.*)接入<b>内建账户</b>、<b>防沉迷</b>、<b>公告</b>、<b>内嵌动态</b>、<b>成就</b>、<b>客服</b>、<b>排行榜</b>、<b>数据埋点</b>几个服务的主要功能和常规接入方式<br>
UE_SDK目前还在开发中，后续项目也将补全相关内容

游戏项目现在使用4.27引擎<br>
适配Win64、Mac、iOS、Android四个平台<br>
专用服务器支持Win64和Linux（目前仅在 Windows 10 和 CentOS 7 中简单测试）<br>
Release页面提供了指定版本的客户端包（缺少iOS包，可自行修改BundleIdentifier并打包）和专用服务器包(Win64和Linux)

SDK仓库：https://github.com/taptap/TapSDK-UE4 <br>
SDK文档：https://developer.taptap.cn/?from=tds-docs <br>

<h3>本项目SDK接入方式</h3>
新增接口类<b>ITdsInterface</b>，接口实现（或者说SDK功能接入）的代码在模块<b>SDKUsage"</b>内。<br>
新增UTdsConfig设置类，包含SDK的初始化配置。<br>

<h3>其它</h3>
如果有什么疑问或者建议可以在开发者中心提交工单或者在游戏的客服系统中提交工单